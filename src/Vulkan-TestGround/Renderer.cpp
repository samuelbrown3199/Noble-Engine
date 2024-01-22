﻿//> includes
#include "Renderer.h"

#include <SDL/SDL.h>
#include <SDL/SDL_vulkan.h>

#include "VulkanInitialisers.h"
#include "VulkanTypes.h"
#include "VulkanImages.h"

#include <chrono>
#include <thread>

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#include "VkBootstrap.h"

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

    vkutil::CopyImageToImage(cmd, m_drawImage.m_image, m_swapchainImages[swapchainImageIndex], m_drawExtent, m_swapchainExtent);

    // set swapchain image layout to Present so we can show it on the screen
    vkutil::TransitionImage(cmd, m_swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

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

        // do not draw if we are minimized
        if (m_bStopRendering) {
            // throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

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
    vkCmdClearColorImage(cmd, m_drawImage.m_image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
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
}