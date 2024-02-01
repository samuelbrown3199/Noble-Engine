//> includes
#include "Renderer.h"

#include <SDL/SDL.h>
#include <SDL/SDL_vulkan.h>

#include "VulkanInitialisers.h"
#include "VulkanTypes.h"
#include "VulkanImages.h"
#include "VulkanPipelines.h"

#include <chrono>
#include <thread>

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#include "VkBootstrap.h"

#include "../src/Engine/imgui/imgui.h"
#include "../src/Engine/imgui/backends/imgui_impl_sdl2.h"
#include "../src/Engine/imgui/backends/imgui_impl_vulkan.h"

Renderer* loadedEngine = nullptr;

Renderer& Renderer::Get() { return *loadedEngine; }
void Renderer::init()
{
    // only one engine initialization is allowed with the application.
    assert(loadedEngine == nullptr);
    loadedEngine = this;

    // We initialize SDL and create a window with it.
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

    m_window = SDL_CreateWindow(
        "Vulkan Engine",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        m_windowExtent.width,
        m_windowExtent.height,
        window_flags);

    InitializeVulkan();
    InitializeSwapchain();
    InitializeCommands();
    InitializeSyncStructures();
    InitializeDescriptors();
    InitializePipelines();

    InitializeImgui();

    // everything went fine
    m_bIsInitialized = true;
}

void Renderer::cleanup()
{
    if (m_bIsInitialized)
    {
        vkDeviceWaitIdle(m_device);

        m_mainDeletionQueue.flush();

        for (int i = 0; i < FRAME_OVERLAP; i++) {
            vkDestroyCommandPool(m_device, m_frames[i].m_commandPool, nullptr);

            vkDestroyFence(m_device, m_frames[i].m_renderFence, nullptr);
            vkDestroySemaphore(m_device, m_frames[i].m_renderSemaphore, nullptr);
            vkDestroySemaphore(m_device, m_frames[i].m_swapchainSemaphore, nullptr);
        }

        DestroySwapchain();

        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkDestroyDevice(m_device, nullptr);

        vkb::destroy_debug_utils_messenger(m_instance, m_debugMessenger);
        vkDestroyInstance(m_instance, nullptr);

        SDL_DestroyWindow(m_window);
    }

    // clear engine pointer
    loadedEngine = nullptr;
}

void Renderer::draw()
{
    if (vkWaitForFences(m_device, 1, &GetCurrentFrame().m_renderFence, true, 1000000000) != VK_SUCCESS)
        throw std::exception();

    GetCurrentFrame().m_deletionQueue.flush();

    if (vkResetFences(m_device, 1, &GetCurrentFrame().m_renderFence) != VK_SUCCESS)
        throw std::exception();

    uint32_t swapchainImageIndex;
    if (vkAcquireNextImageKHR(m_device, m_swapchain, 100000000, GetCurrentFrame().m_swapchainSemaphore, nullptr, &swapchainImageIndex) != VK_SUCCESS)
        throw std::exception();

    VkCommandBuffer cmd = GetCurrentFrame().m_mainCommandBuffer;
    if (vkResetCommandBuffer(cmd, 0) != VK_SUCCESS)
        throw std::exception();

    m_drawExtent.width = m_drawImage.m_imageExtent.width;
    m_drawExtent.height = m_drawImage.m_imageExtent.height;

    VkCommandBufferBeginInfo cmdBeginInfo = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    if (vkBeginCommandBuffer(cmd, &cmdBeginInfo) != VK_SUCCESS)
        throw std::exception();

    vkutil::TransitionImage(cmd, m_drawImage.m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    DrawBackground(cmd);

    vkutil::TransitionImage(cmd, m_drawImage.m_image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    vkutil::TransitionImage(cmd, m_swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    vkutil::CopyImageToImage(cmd, m_drawImage.m_image, m_swapchainImages[swapchainImageIndex], m_drawExtent, m_swapchainExtent); // I want an option at some point to skip this stuff and to copy the image into an imgui window.

    vkutil::TransitionImage(cmd, m_swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    DrawImGui(cmd, m_swapchainImageViews[swapchainImageIndex]);

    // set swapchain image layout to Present so we can show it on the screen
    vkutil::TransitionImage(cmd, m_swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
        throw std::exception();


    VkCommandBufferSubmitInfo cmdInfo = vkinit::CommandBufferSubmitInfo(cmd);

    VkSemaphoreSubmitInfo waitInfo = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, GetCurrentFrame().m_swapchainSemaphore);
    VkSemaphoreSubmitInfo signalInfo = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, GetCurrentFrame().m_renderSemaphore);

    VkSubmitInfo2 submit = vkinit::SubmitInfo(&cmdInfo, &signalInfo, &waitInfo);

    if (vkQueueSubmit2(m_graphicsQueue, 1, &submit, GetCurrentFrame().m_renderFence) != VK_SUCCESS)
        throw std::exception();

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &m_swapchain;
    presentInfo.swapchainCount = 1;

    presentInfo.pWaitSemaphores = &GetCurrentFrame().m_renderSemaphore;
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &swapchainImageIndex;

    if (vkQueuePresentKHR(m_graphicsQueue, &presentInfo) != VK_SUCCESS)
        throw std::exception();

    m_iFrameNumber++;
}

void Renderer::DrawImGui(VkCommandBuffer cmd, VkImageView targetImageView)
{
    VkRenderingAttachmentInfo colorAttachment = vkinit::AttachmentInfo(targetImageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);
    VkRenderingInfo renderInfo = vkinit::RenderingInfo(m_swapchainExtent, &colorAttachment, nullptr);

    vkCmdBeginRendering(cmd, &renderInfo);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

    vkCmdEndRendering(cmd);
}

void Renderer::run()
{
    SDL_Event e;
    bool bQuit = false;

    // main loop
    while (!bQuit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // close the window when user alt-f4s or clicks the X button
            if (e.type == SDL_QUIT)
                bQuit = true;

            if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                    m_bStopRendering = true;
                }
                if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
                    m_bStopRendering = false;
                }
            }
        }

        //send SDL event to imgui for handling
        ImGui_ImplSDL2_ProcessEvent(&e);

        // do not draw if we are minimized
        if (m_bStopRendering) {
            // throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // imgui new frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame(m_window);
        ImGui::NewFrame();

        if (ImGui::Begin("background")) {

            ComputeEffect& selected = backgroundEffects[currentBackgroundEffect];

            ImGui::Text("Selected effect: ", selected.name);

            ImGui::SliderInt("Effect Index", &currentBackgroundEffect, 0, backgroundEffects.size() - 1);

            ImGui::InputFloat4("data1", (float*)&selected.data.data1);
            ImGui::InputFloat4("data2", (float*)&selected.data.data2);
            ImGui::InputFloat4("data3", (float*)&selected.data.data3);
            ImGui::InputFloat4("data4", (float*)&selected.data.data4);

            ImGui::End();
        }

        //make imgui calculate internal draw structures
        ImGui::Render();

        draw();
    }
}


void Renderer::InitializeVulkan()
{
    vkb::InstanceBuilder builder;

    auto instanceRet = builder.set_app_name("Vulkan App")
        .request_validation_layers(m_bUseValidationLayer) //Probably use the same debug build logic in the current renderer, maybe also include a new setting to turn this on in release builds.
        .use_default_debug_messenger() //Might need to add the callback from the current renderer to get proper engine logging.
        .require_api_version(1, 3, 0)
        .build();

    vkb::Instance vkbInstance = instanceRet.value();

    m_instance = vkbInstance.instance;
    m_debugMessenger = vkbInstance.debug_messenger;

    SDL_Vulkan_CreateSurface(m_window, m_instance, &m_surface);

    //Vulkan 1.3 Features
    VkPhysicalDeviceVulkan13Features features{};
    features.dynamicRendering = true;
    features.synchronization2 = true;

    //Vulkan 1.2 Features
    VkPhysicalDeviceVulkan12Features features12{};
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;

    //Using VkBootstrap to select a gpu.
    //Supports some 1.3 & 1.2 features and also supplies the surface for writing to.
    vkb::PhysicalDeviceSelector selector{ vkbInstance };
    vkb::PhysicalDevice physicalDevice = selector
        .set_minimum_version(1, 3)
        .set_required_features_13(features)
        .set_required_features_12(features12)
        .set_surface(m_surface)
        .select()
        .value();

    vkb::DeviceBuilder deviceBuilder{ physicalDevice };
    vkb::Device vkbDevice = deviceBuilder.build().value();

    m_device = vkbDevice.device;
    m_physicalDevice = physicalDevice.physical_device;

    //Use VkBootstrap to get a Graphics Queue
    m_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    m_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = m_physicalDevice;
    allocatorInfo.device = m_device;
    allocatorInfo.instance = m_instance;
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    vmaCreateAllocator(&allocatorInfo, &m_allocator);

    m_mainDeletionQueue.push_function([&]() {
        vmaDestroyAllocator(m_allocator);
    });
}

void Renderer::CreateSwapchain(uint32_t width, uint32_t height)
{
    vkb::SwapchainBuilder swapchainBuilder{ m_physicalDevice, m_device, m_surface };
    m_swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

    vkb::Swapchain vkbSwapchain = swapchainBuilder
        .set_desired_format(VkSurfaceFormatKHR{ .format = m_swapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) //vsync setting needs to be considered here.
        .set_desired_extent(width, height)
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build()
        .value();

    m_swapchainExtent = vkbSwapchain.extent;
    m_swapchain = vkbSwapchain.swapchain;
    m_swapchainImages = vkbSwapchain.get_images().value();
    m_swapchainImageViews = vkbSwapchain.get_image_views().value();
}

void Renderer::DestroySwapchain()
{
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

    for (int i = 0; i < m_swapchainImageViews.size(); i++)
    {
        vkDestroyImageView(m_device, m_swapchainImageViews[i], nullptr);
    }
}

void Renderer::DrawBackground(VkCommandBuffer cmd)
{
    VkClearColorValue clearValue;
    float flash = abs(sin(m_iFrameNumber / 120.0f));
    clearValue = { { 0.0f, 0.0f, flash, 1.0f } };

    VkImageSubresourceRange clearRange = vkinit::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);
    
    ComputeEffect& effect = backgroundEffects[currentBackgroundEffect];

    //Bind gradient drawing compute pipeline
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, effect.pipeline); //bind relevant pipeline

    // bind the descriptor set containing the draw image for the compute pipeline
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_gradientPipelineLayout, 0, 1, &m_drawImageDescriptors, 0, nullptr);

    //Send the push constant value. Requires command buffer, pipeline layout, stage, offset, size and reference to data
    vkCmdPushConstants(cmd, m_gradientPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstants), &effect.data);

    // execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it
    vkCmdDispatch(cmd, std::ceil(m_drawExtent.width / 16.0), std::ceil(m_drawExtent.height / 16.0), 1);
}

void Renderer::InitializeSwapchain()
{
    CreateSwapchain(m_windowExtent.width, m_windowExtent.height);

    //draw image size will match the window
    VkExtent3D drawImageExtent = {
        m_windowExtent.width,
        m_windowExtent.height,
        1
    };

    //hardcoding the draw format to 32 bit float
    m_drawImage.m_imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    m_drawImage.m_imageExtent = drawImageExtent;

    VkImageUsageFlags drawImageUsages{};
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkImageCreateInfo rimgInfo = vkinit::ImageCreateInfo(m_drawImage.m_imageFormat, drawImageUsages, drawImageExtent);
    
    VmaAllocationCreateInfo rimgAllocInfo = {};
    rimgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    rimgAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vmaCreateImage(m_allocator, &rimgInfo, &rimgAllocInfo, &m_drawImage.m_image, &m_drawImage.m_allocation, nullptr);

    VkImageViewCreateInfo rviewInfo = vkinit::ImageViewCreateInfo(m_drawImage.m_imageFormat, m_drawImage.m_image, VK_IMAGE_ASPECT_COLOR_BIT);

    if (vkCreateImageView(m_device, &rviewInfo, nullptr, &m_drawImage.m_imageView) != VK_SUCCESS)
        throw std::exception();

    m_mainDeletionQueue.push_function([=]()
    {
        vkDestroyImageView(m_device, m_drawImage.m_imageView, nullptr);
        vmaDestroyImage(m_allocator, m_drawImage.m_image, m_drawImage.m_allocation);
    });
}

void Renderer::InitializeCommands()
{
    //Create a command pool for commands submitted to the graphics queue.
    //We also want the pool to allow for resetting of individual command buffers.
    VkCommandPoolCreateInfo commandPoolInfo = vkinit::CommandPoolCreateInfo(m_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    for (int i = 0; i < FRAME_OVERLAP; i++)
    {
        if (vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_frames[i].m_commandPool) != VK_SUCCESS)
            throw std::exception();

        VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::CommandBufferAllocateInfo(m_frames[i].m_commandPool, 1);
        if (vkAllocateCommandBuffers(m_device, &cmdAllocInfo, &m_frames[i].m_mainCommandBuffer) != VK_SUCCESS)
            throw std::exception();
    }

    if (vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_immediateCommandPool) != VK_SUCCESS)
        throw std::exception();

    VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::CommandBufferAllocateInfo(m_immediateCommandPool, 1);
    if (vkAllocateCommandBuffers(m_device, &cmdAllocInfo, &m_immediateCommandBuffer) != VK_SUCCESS)
        throw std::exception();

    m_mainDeletionQueue.push_function([=]() {
        vkDestroyCommandPool(m_device, m_immediateCommandPool, nullptr);
    });
}

void Renderer::InitializeSyncStructures()
{	
    //create syncronization structures
    //one fence to control when the gpu has finished rendering the frame,
    //and 2 semaphores to syncronize rendering with swapchain
    //we want the fence to start signalled so we can wait on it on the first frame
    VkFenceCreateInfo fenceCreateInfo = vkinit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
    VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::SemaphoreCreateInfo();

    for (int i = 0; i < FRAME_OVERLAP; i++)
    {
        if (vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_frames[i].m_renderFence) != VK_SUCCESS)
            throw std::exception();

        if (vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_frames[i].m_swapchainSemaphore) != VK_SUCCESS)
            throw std::exception();
        if (vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_frames[i].m_renderSemaphore) != VK_SUCCESS)
            throw std::exception();
    }

    if (vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_immediateFence) != VK_SUCCESS)
        throw std::exception();

    m_mainDeletionQueue.push_function([=]() { vkDestroyFence(m_device, m_immediateFence, nullptr); });
}

void Renderer::InitializeDescriptors()
{
    std::vector<DescriptorAllocator::PoolSizeRatio> sizes =
    {
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
    };

    m_globalDescriptorAllocator.InitializePool(m_device, 10, sizes);

    {
        DescriptorLayoutBuilder builder;
        builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);

        m_drawImageDescriptorLayout = builder.Build(m_device, VK_SHADER_STAGE_COMPUTE_BIT);
    }

    m_drawImageDescriptors = m_globalDescriptorAllocator.Allocate(m_device, m_drawImageDescriptorLayout);

    VkDescriptorImageInfo imgInfo = {};
    imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    imgInfo.imageView = m_drawImage.m_imageView;

    VkWriteDescriptorSet drawImageWrite = {};
    drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    drawImageWrite.pNext = nullptr;

    drawImageWrite.dstBinding = 0;
    drawImageWrite.dstSet = m_drawImageDescriptors;
    drawImageWrite.descriptorCount = 1;
    drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    drawImageWrite.pImageInfo = &imgInfo;

    vkUpdateDescriptorSets(m_device, 1, &drawImageWrite, 0, nullptr);
}

void Renderer::InitializePipelines()
{
    InitializeBackgroundPipelines();
}

void Renderer::InitializeBackgroundPipelines()
{
    VkPipelineLayoutCreateInfo computeLayout{};
    computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    computeLayout.pNext = nullptr;
    computeLayout.pSetLayouts = &m_drawImageDescriptorLayout;
    computeLayout.setLayoutCount = 1;

    VkPushConstantRange pushConstant = {};
    pushConstant.offset = 0;
    pushConstant.size = sizeof(ComputePushConstants);
    pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    computeLayout.pPushConstantRanges = &pushConstant;
    computeLayout.pushConstantRangeCount = 1;

    if (vkCreatePipelineLayout(m_device, &computeLayout, nullptr, &m_gradientPipelineLayout) != VK_SUCCESS)
        throw std::exception();

    VkShaderModule gradientShader;
    if (!vkutil::LoadShaderModule("gradient.spv", m_device, &gradientShader))
    {
        throw std::exception();
    }

    VkShaderModule skyShader;
    if (!vkutil::LoadShaderModule("sky.spv", m_device, &skyShader))
    {
        throw std::exception();
    }

    VkPipelineShaderStageCreateInfo stageInfo{};
    stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageInfo.pNext = nullptr;
    stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stageInfo.module = gradientShader;
    stageInfo.pName = "main";

    VkComputePipelineCreateInfo computePipelineCreateInfo{};
    computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineCreateInfo.pNext = nullptr;
    computePipelineCreateInfo.layout = m_gradientPipelineLayout;
    computePipelineCreateInfo.stage = stageInfo;

    ComputeEffect gradient;
    gradient.layout = m_gradientPipelineLayout;
    gradient.name = "gradient";
    gradient.data = {};

    //default colors
    gradient.data.data1 = glm::vec4(1, 0, 0, 1);
    gradient.data.data2 = glm::vec4(0, 0, 1, 1);

    if (vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &gradient.pipeline) != VK_SUCCESS)
        throw std::exception();

    //change the shader module only to create the sky shader
    computePipelineCreateInfo.stage.module = skyShader;

    ComputeEffect sky;
    sky.layout = m_gradientPipelineLayout;
    sky.name = "sky";
    sky.data = {};
    //default sky parameters
    sky.data.data1 = glm::vec4(0.1, 0.2, 0.4, 0.97);

    if (vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &sky.pipeline) != VK_SUCCESS)
        throw std::exception();

    backgroundEffects.push_back(gradient);
    backgroundEffects.push_back(sky);

    vkDestroyShaderModule(m_device, gradientShader, nullptr);
    vkDestroyShaderModule(m_device, skyShader, nullptr);

    m_mainDeletionQueue.push_function([&]()
        {
            vkDestroyPipelineLayout(m_device, m_gradientPipelineLayout, nullptr);
            vkDestroyPipeline(m_device, sky.pipeline, nullptr);
            vkDestroyPipeline(m_device, gradient.pipeline, nullptr);
        });
}

void Renderer::InitializeImgui()
{
    // 1: create descriptor pool for IMGUI
    //  the size of the pool is very oversize, but it's copied from imgui demo
    //  itself.
    VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VkDescriptorPool imguiPool;
    if (vkCreateDescriptorPool(m_device, &pool_info, nullptr, &imguiPool) != VK_SUCCESS)
        throw std::exception();

    // 2: initialize imgui library

    // this initializes the core structures of imgui
    ImGui::CreateContext();

    // this initializes imgui for SDL
    ImGui_ImplSDL2_InitForVulkan(m_window);

    // this initializes imgui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = m_instance;
    init_info.PhysicalDevice = m_physicalDevice;
    init_info.Device = m_device;
    init_info.Queue = m_graphicsQueue;
    init_info.DescriptorPool = imguiPool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.UseDynamicRendering = true;
    init_info.ColorAttachmentFormat = m_swapchainImageFormat;

    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, VK_NULL_HANDLE);

    // execute a gpu command to upload imgui font textures
    ImGui_ImplVulkan_CreateFontsTexture();

    // add the destroy the imgui created structures
    m_mainDeletionQueue.push_function([=]() {
        vkDestroyDescriptorPool(m_device, imguiPool, nullptr);
        ImGui_ImplVulkan_Shutdown();
    });
}

void Renderer::ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function)
{
    if (vkResetFences(m_device, 1, &m_immediateFence) != VK_SUCCESS)
        throw std::exception();
    if (vkResetCommandBuffer(m_immediateCommandBuffer, 0) != VK_SUCCESS)
        throw std::exception();

    VkCommandBuffer cmd = m_immediateCommandBuffer;

    VkCommandBufferBeginInfo cmdBeginInfo = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    if (vkBeginCommandBuffer(cmd, &cmdBeginInfo) != VK_SUCCESS)
        throw std::exception();

    function(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
        throw std::exception();

    VkCommandBufferSubmitInfo cmdInfo = vkinit::CommandBufferSubmitInfo(cmd);
    VkSubmitInfo2 submit = vkinit::SubmitInfo(&cmdInfo, nullptr, nullptr);

    if (vkQueueSubmit2(m_graphicsQueue, 1, &submit, m_immediateFence) != -VK_SUCCESS)
        throw std::exception();

    if (vkWaitForFences(m_device, 1, &m_immediateFence, true, 99999999) != VK_SUCCESS)
        throw std::exception();
}