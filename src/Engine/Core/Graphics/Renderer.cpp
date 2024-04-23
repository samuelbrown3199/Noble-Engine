#include "Renderer.h"

#include "../VersionInformation.h"

#include "../ECS/Renderable.hpp"

#include <set>

#include "VulkanInitialisers.h"
#include "VulkanTypes.h"
#include "VulkanImages.h"
#include "VulkanPipelines.h"

#include "../Logger.h"
#include "../../Useful.h"
#include "../EngineComponents/Sprite.h"

#include "../EngineResources/Pipeline.h"

#include "../../imgui/imgui.h"
#include "../../imgui/backends/imgui_impl_vulkan.h"
#include "../../imgui/backends/imgui_impl_sdl2.h"
#include "../../imgui/implot.h"

#include "../Registry.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/hash.hpp>

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#include "VkBootstrap.h"

//---------- public functions ---------

Renderer::Renderer(const std::string _windowName)
{
	LogInfo("Creating engine renderer.");

	SDL_DisplayMode displayMode;
	SDL_GetDesktopDisplayMode(0, &displayMode);

	m_iScreenHeight = displayMode.h;
	m_iScreenWidth = displayMode.w;

	m_gameWindow = SDL_CreateWindow(_windowName.c_str(), 50, 50, m_iScreenWidth, m_iScreenHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED);
	if (!m_gameWindow)
	{
		LogFatalError(FormatString("Failed to create game window %s", SDL_GetError()));
	}
	LogTrace("Created game window.");

	InitializeVulkan();
}

Renderer::~Renderer()
{
	delete m_camera;

	CleanupVulkan();
}

void Renderer::WaitForRenderingToFinish()
{
	vkDeviceWaitIdle(m_device);
}

void Renderer::InitializeVulkan()
{
	LogInfo("Initializing Vulkan");

	CreateInstance();
	InitializeSwapchain();
	InitializeCommands();
	InitializeSyncStructures();
	InitializeDescriptors();

	InitializeDefaultData();
	InitializeImgui();

	Renderer::SetClearColour(glm::vec3(0.0f, 0.25, 0.75));

	LogInfo("Initialized Vulkan");
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

void Renderer::CleanupVulkan()
{
	vkDeviceWaitIdle(m_device);

	DestroyImage(m_drawImage);
	DestroyImage(m_depthImage);

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyCommandPool(m_device, m_frames[i].m_commandPool, nullptr);

		vkDestroyFence(m_device, m_frames[i].m_renderFence, nullptr);
		vkDestroySemaphore(m_device, m_frames[i].m_renderSemaphore, nullptr);
		vkDestroySemaphore(m_device, m_frames[i].m_swapchainSemaphore, nullptr);

		m_frames[i].m_deletionQueue.flush();
		m_frames[i].m_frameDescriptors.DestroyPools(m_device);
	}

	m_mainDeletionQueue.flush();
	DestroySwapchain();

	vmaDestroyAllocator(m_allocator);

	vkDestroySurfaceKHR(m_vulkanInstance, m_surface, nullptr);
	vkDestroyDevice(m_device, nullptr);

	vkb::destroy_debug_utils_messenger(m_vulkanInstance, m_debugMessenger);
	vkDestroyInstance(m_vulkanInstance, nullptr);

	SDL_DestroyWindow(m_gameWindow);
}

void Renderer::CreateInstance()
{
	vkb::InstanceBuilder builder;

	auto instanceRet = builder.set_app_name(GetWindowTitle().c_str())
		.request_validation_layers(m_bEnableValidationLayers) //Probably use the same debug build logic in the current renderer, maybe also include a new setting to turn this on in release builds.
		.require_api_version(1, 3, 0)
		.set_debug_callback(&Renderer::DebugCallback)
		.set_engine_name("Noble")
		.set_engine_version(VK_MAKE_API_VERSION(VERSION_STAGE, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH))
		.build();

	vkb::Instance vkbInstance = instanceRet.value();

	m_vulkanInstance = vkbInstance.instance;
	m_debugMessenger = vkbInstance.debug_messenger;

	SDL_Vulkan_CreateSurface(m_gameWindow, m_vulkanInstance, &m_surface);

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
	allocatorInfo.instance = m_vulkanInstance;
	allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	vmaCreateAllocator(&allocatorInfo, &m_allocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	LogInfo(FormatString("Validation Layer: %s", pCallbackData->pMessage));

	return VK_FALSE;
}

void Renderer::InitializeSwapchain()
{
	CreateSwapchain(m_iScreenWidth, m_iScreenHeight);

	//draw image size will match the window
	VkExtent3D drawImageExtent = {
		m_iScreenWidth,
		m_iScreenHeight,
		1
	};

	VkImageUsageFlags drawImageUsages{};
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_SAMPLED_BIT;

	m_drawImage = CreateImage(drawImageExtent, VK_FORMAT_R16G16B16A16_SFLOAT, drawImageUsages, false, "DrawImage");

	VkImageUsageFlags depthImageUsages{};
	depthImageUsages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	m_depthImage = CreateImage(drawImageExtent, VK_FORMAT_D32_SFLOAT, depthImageUsages, false, "DepthImage");
}

void Renderer::RecreateSwapchain()
{
	vkDeviceWaitIdle(m_device);

	DestroyImage(m_drawImage);
	DestroyImage(m_depthImage);

	DestroySwapchain();
	InitializeSwapchain();
}

void Renderer::InitializeCommands()
{
	//Create a command pool for commands submitted to the graphics queue.
	//We also want the pool to allow for resetting of individual command buffers.
	VkCommandPoolCreateInfo commandPoolInfo = vkinit::CommandPoolCreateInfo(m_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
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

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
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

	DescriptorWriter writer;
	writer.WriteImage(0, m_drawImage.m_imageView, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
	writer.UpdateSet(m_device, m_drawImageDescriptors);

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> frameSizes =
		{
			{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3},
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 },
		};

		m_frames[i].m_frameDescriptors = DescriptorAllocatorGrowable{};
		m_frames[i].m_frameDescriptors.Initialize(m_device, 1000, frameSizes);

		m_mainDeletionQueue.push_function([&, i]()
			{
				m_frames[i].m_frameDescriptors.DestroyPools(m_device);
			});
	}

	{
		DescriptorLayoutBuilder builder;
		builder.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		m_gpuSceneDataDescriptorLayout = builder.Build(m_device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	{
		DescriptorLayoutBuilder builder;
		builder.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		m_singleImageDescriptorLayout = builder.Build(m_device, VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	RegisterDescriptors();

	m_mainDeletionQueue.push_function([=]()
	{
		vkDestroyDescriptorSetLayout(m_device, m_drawImageDescriptorLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_device, m_gpuSceneDataDescriptorLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_device, m_singleImageDescriptorLayout, nullptr);

		m_globalDescriptorAllocator.ClearDescriptors(m_device);
		m_globalDescriptorAllocator.DestroyPool(m_device);
	});
}

void Renderer::RegisterDescriptors()
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	registry->RegisterDescriptor("DrawImage", &m_drawImageDescriptorLayout, &m_drawImageDescriptors, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
	registry->RegisterDescriptor("SingleImage", &m_singleImageDescriptorLayout, nullptr, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	registry->RegisterDescriptor("GPUSceneData", &m_gpuSceneDataDescriptorLayout, nullptr, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

	registry->RegisterPushConstant<GPUDrawPushConstants>("GPUDrawData", VK_SHADER_STAGE_VERTEX_BIT);
}

SwapChainSupportDetails Renderer::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR Renderer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR Renderer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (m_bVsync)
		{
			if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR)
			{
				return availablePresentMode;
			}
		}
		else
		{
			if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				return availablePresentMode;
			}
		}
	}

	return VK_PRESENT_MODE_MAILBOX_KHR;
}

void Renderer::CreateSwapchain(uint32_t width, uint32_t height)
{
	vkb::SwapchainBuilder swapchainBuilder{ m_physicalDevice, m_device, m_surface };
	m_swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

	SwapChainSupportDetails swapchainSupport = QuerySwapChainSupport(m_physicalDevice);

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.set_desired_format(VkSurfaceFormatKHR{ .format = m_swapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		.set_desired_present_mode(ChooseSwapPresentMode(swapchainSupport.presentModes))
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

//----------------------------------------------------------------------------------------------------------------------------------------------

VkFormat Renderer::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	LogFatalError("Failed to find supported format.");
	VkFormat error;
	return error;
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
	m_imguiContext = ImGui::CreateContext();
	ImPlot::CreateContext();

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// this initializes imgui for SDL
	ImGui_ImplSDL2_InitForVulkan(m_gameWindow);

	// this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = m_vulkanInstance;
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
		ImPlot::DestroyContext();
		ImGui_ImplVulkan_Shutdown();
		
		vkDestroyDescriptorPool(m_device, imguiPool, nullptr);
	});
}

void Renderer::ResetForNextFrame()
{
	m_iRenderableCount = 0;
	m_onScreenObjects.clear();
}

void Renderer::DrawFrame()
{
	if (vkWaitForFences(m_device, 1, &GetCurrentFrame().m_renderFence, true, 1000000000) != VK_SUCCESS)
		LogFatalError("Failed to wait for fence.");

	GetCurrentFrame().m_deletionQueue.flush();
	GetCurrentFrame().m_frameDescriptors.ClearPools(m_device);

	if (vkResetFences(m_device, 1, &GetCurrentFrame().m_renderFence) != VK_SUCCESS)
		LogFatalError("Failed to reset fence.");

	uint32_t swapchainImageIndex;
	if (vkAcquireNextImageKHR(m_device, m_swapchain, 100000000, GetCurrentFrame().m_swapchainSemaphore, nullptr, &swapchainImageIndex) != VK_SUCCESS)
		LogFatalError("Failed to acquire next image.");

	VkCommandBuffer cmd = GetCurrentFrame().m_mainCommandBuffer;
	if (vkResetCommandBuffer(cmd, 0) != VK_SUCCESS)
		LogFatalError("Failed to reset command buffer.");

	if (m_fRenderScale == 0)
		LogFatalError("Render scale is set to 0. This should never happen.");

	glm::vec3 viewPos = glm::vec3(0, 0, 0);
	if (m_camera != nullptr)
	{
		NobleRegistry* registry = Application::GetApplication()->GetRegistry();
		viewPos = registry->GetComponent<Transform>(m_camera->m_camTransformIndex)->m_position;

		switch (m_camera->m_iDrawMode)
		{
		case 0:
			m_drawFilter = VK_FILTER_NEAREST;
			break;
		case 1:
			m_drawFilter = VK_FILTER_LINEAR;
			break;
		default:
			LogError("Invalid draw mode.");
			break;
		}
	}

	m_drawExtent.width = m_drawImage.m_imageExtent.width * m_fRenderScale;
	m_drawExtent.height = m_drawImage.m_imageExtent.height * m_fRenderScale;

	m_sceneData.viewPos = viewPos;
	m_sceneData.proj = GenerateProjMatrix();
	m_sceneData.view = GenerateViewMatrix();
	m_sceneData.viewproj = m_sceneData.proj * m_sceneData.view;

	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	if (vkBeginCommandBuffer(cmd, &cmdBeginInfo) != VK_SUCCESS)
		throw std::exception();

	vkutil::TransitionImage(cmd, m_drawImage.m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	DrawBackground(cmd);

	vkutil::TransitionImage(cmd, m_drawImage.m_image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	vkutil::TransitionImage(cmd, m_depthImage.m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

	DrawGeometry(cmd);

	vkutil::TransitionImage(cmd, m_drawImage.m_image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	vkutil::TransitionImage(cmd, m_swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	if(!m_bDrawToWindow)
		vkutil::CopyImageToImage(cmd, m_drawImage.m_image, m_swapchainImages[swapchainImageIndex], m_drawExtent, m_swapchainExtent, m_drawFilter);
	else
	{
		vkutil::TransitionImage(cmd, m_drawImage.m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
		if (*m_drawWindowSet == VK_NULL_HANDLE)
			*m_drawWindowSet = ImGui_ImplVulkan_AddTexture(m_defaultSamplerLinear, m_drawImage.m_imageView, VK_IMAGE_LAYOUT_GENERAL);
	}

	DrawImGui(cmd, m_swapchainImages[swapchainImageIndex], m_swapchainImageViews[swapchainImageIndex]);

	// set swapchain image layout to Present so we can show it on the screen
	vkutil::TransitionImage(cmd, m_swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
		LogFatalError("Failed to end command buffer.");

	VkCommandBufferSubmitInfo cmdInfo = vkinit::CommandBufferSubmitInfo(cmd);

	VkSemaphoreSubmitInfo waitInfo = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, GetCurrentFrame().m_swapchainSemaphore);
	VkSemaphoreSubmitInfo signalInfo = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, GetCurrentFrame().m_renderSemaphore);

	VkSubmitInfo2 submit = vkinit::SubmitInfo(&cmdInfo, &signalInfo, &waitInfo);

	if (vkQueueSubmit2(m_graphicsQueue, 1, &submit, GetCurrentFrame().m_renderFence) != VK_SUCCESS)
		LogFatalError("Failed to submit queue.");

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.pSwapchains = &m_swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &GetCurrentFrame().m_renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	VkResult result = vkQueuePresentKHR(m_graphicsQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR && result != VK_SUBOPTIMAL_KHR)
	{
		RecreateSwapchain();
	}
	else if (result != VK_SUCCESS)
	{
		LogFatalError("Failed to present swap chain image.");
	}

	m_iCurrentFrame = (m_iCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::DrawBackground(VkCommandBuffer cmd)
{
	VkClearColorValue clearValue;
	clearValue = { { m_clearColour.x, m_clearColour.y, m_clearColour.z, 1.0f } };

	VkImageSubresourceRange clearRange = vkinit::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

	vkCmdClearColorImage(cmd, m_drawImage.m_image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
}

void Renderer::DrawGeometry(VkCommandBuffer cmd)
{
	AllocatedBuffer gpuSceneDataBuffer = CreateBuffer(sizeof(GPUSceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, "GPUSceneBuffer");
	GetCurrentFrame().m_deletionQueue.push_function([=, this]()
	{
		DestroyBuffer(gpuSceneDataBuffer);
	});

	GPUSceneData* sceneUniformData = (GPUSceneData*)gpuSceneDataBuffer.m_allocation->GetMappedData();
	*sceneUniformData = m_sceneData;

	GetCurrentFrame().m_sceneDescriptor = GetCurrentFrame().m_frameDescriptors.AllocateSet(m_device, m_gpuSceneDataDescriptorLayout);

	DescriptorWriter writer;
	writer.WriteBuffer(0, gpuSceneDataBuffer.m_buffer, sizeof(GPUSceneData), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	writer.UpdateSet(m_device, GetCurrentFrame().m_sceneDescriptor);

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	registry->GetDescriptorFromName("GPUSceneData")->m_set = &GetCurrentFrame().m_sceneDescriptor;

	//Begin a render pass connected to our draw image. 
	VkRenderingAttachmentInfo colorAttachment = vkinit::AttachmentInfo(m_drawImage.m_imageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);
	VkRenderingAttachmentInfo depthAttachment = vkinit::DepthAttachmentInfo(m_depthImage.m_imageView, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
	VkRenderingInfo renderInfo = vkinit::RenderingInfo(m_drawExtent, &colorAttachment, &depthAttachment);
	vkCmdBeginRendering(cmd, &renderInfo);

	//set dynamic viewport and scissor
	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = m_drawExtent.width;
	viewport.height = m_drawExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	vkCmdSetViewport(cmd, 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent.width = m_drawExtent.width;
	scissor.extent.height = m_drawExtent.height;

	vkCmdSetScissor(cmd, 0, 1, &scissor);

	for (int i = 0; i < m_onScreenObjects.size(); i++)
	{
		m_onScreenObjects.at(i)->OnRender(cmd);
	}

	vkCmdEndRendering(cmd);
}
void Renderer::DrawImGui(VkCommandBuffer cmd, VkImage targetImage, VkImageView targetImageView)
{
	//make imgui calculate internal draw structures
	ImGui::Render();

	VkRenderingAttachmentInfo colorAttachment = vkinit::AttachmentInfo(targetImageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);
	VkRenderingInfo renderInfo = vkinit::RenderingInfo(m_swapchainExtent, &colorAttachment, nullptr);

	if (m_bDrawToWindow)
	{
		VkClearColorValue clearValue;
		clearValue = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		VkImageSubresourceRange clearRange = vkinit::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);
		vkCmdClearColorImage(cmd, targetImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearValue, 1, &clearRange);
	}
	vkutil::TransitionImage(cmd, targetImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	vkCmdBeginRendering(cmd, &renderInfo);

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

	vkCmdEndRendering(cmd);
}

void Renderer::InitializeDefaultData()
{
	//checkerboard image
	glm::ivec3 black = glm::ivec3(0,0,0);
	glm::ivec3 magenta = glm::ivec3(83, 0, 99);
	char* pixelColours = new char[(16*16)*4];
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			int pixelPos = (x + 16 * y) * 4;
			pixelColours[pixelPos] = ((x % 2) ^ (y % 2)) ? magenta.x : black.x;
			pixelColours[pixelPos+1] = ((x % 2) ^ (y % 2)) ? magenta.y : black.y;
			pixelColours[pixelPos + 2] = ((x % 2) ^ (y % 2)) ? magenta.z : black.z;
			pixelColours[pixelPos + 3] = 255;
		}
	}
	m_errorCheckerboardImage = CreateImage(pixelColours, VkExtent3D{ 16, 16, 1 }, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_SAMPLED_BIT, false, "CheckerboardErrorTexture");

	delete[] pixelColours;

	VkSamplerCreateInfo sampl = { .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };

	sampl.magFilter = VK_FILTER_NEAREST;
	sampl.minFilter = VK_FILTER_NEAREST;

	vkCreateSampler(m_device, &sampl, nullptr, &m_defaultSamplerNearest);

	sampl.magFilter = VK_FILTER_LINEAR;
	sampl.minFilter = VK_FILTER_LINEAR;
	vkCreateSampler(m_device, &sampl, nullptr, &m_defaultSamplerLinear);

	m_spriteQuad = UploadMesh(Sprite::spriteQuadIndices, Sprite::spriteQuadVertices, "SpriteQuad");

	m_mainDeletionQueue.push_function([=]()
	{
		vkDestroySampler(m_device, m_defaultSamplerLinear, nullptr);
		vkDestroySampler(m_device, m_defaultSamplerNearest, nullptr);
		DestroyImage(m_errorCheckerboardImage);
	});
}

void Renderer::CheckScreenSizeForUpdate()
{
	int newWidth, newHeight;
	SDL_GetWindowSize(m_gameWindow, &newWidth, &newHeight);
	UpdateScreenSize(newHeight, newWidth);
}

void Renderer::UpdateScreenSize(const int& _height, const int& _width)
{
	if (_height < 0 || _width < 0)
		LogFatalError(FormatString("Trying to resize screen to invalid size, %d %d", _width, _height));

	int newWidth = _width, newHeight = _height;
	if (newHeight == 0 || newWidth == 0)
	{
		SDL_DisplayMode displayMode;
		SDL_GetDesktopDisplayMode(0, &displayMode);

		newHeight = displayMode.h;
		newWidth = displayMode.w;
	}

	if (newWidth != m_iScreenWidth || newHeight != m_iScreenHeight)
	{
		m_iScreenWidth = newWidth;
		m_iScreenHeight = newHeight;

		SDL_SetWindowSize(m_gameWindow, m_iScreenWidth, m_iScreenHeight);
		LogInfo(FormatString("Resized game window to %dx%d.", m_iScreenWidth, m_iScreenHeight));
	}
}

void Renderer::SetWindowFullScreen(const int& _mode)
{
	LogInfo(FormatString("Changing window fullscreen mode to %d", _mode));
	switch (_mode)
	{
	case 0:
		SDL_SetWindowBordered(m_gameWindow, SDL_TRUE);
		SDL_SetWindowFullscreen(m_gameWindow, 0);
		break;
	case 2:
		SDL_SetWindowBordered(m_gameWindow, SDL_FALSE);
	case 1:
		SDL_SetWindowFullscreen(m_gameWindow, SDL_WINDOW_FULLSCREEN);
		break;
	}

	Renderer::CheckScreenSizeForUpdate();
}

std::string Renderer::GetWindowTitle()
{
	std::string name(SDL_GetWindowTitle(m_gameWindow));
	return name;
}

// 0 No VSync, 1 VSync
void Renderer::SetVSyncMode(const int& _mode)
{
	m_bVsync = _mode;
	RecreateSwapchain();
	LogInfo(FormatString("Changing VSync mode to %d", _mode));
}

void Renderer::SetRenderScale(const float& value)
{
	if (m_fRenderScale == value)
		return;
	
	m_fRenderScale = value;
	LogInfo(FormatString("Set render scale to %.2f", m_fRenderScale));
}

glm::mat4 Renderer::GenerateProjMatrix()
{
	if (m_camera == nullptr)
		return glm::mat4(1.0f);

	if (m_camera->m_viewMode == projection)
		return GenerateProjectionMatrix();

	return GenerateOrthographicMatrix();
}

glm::mat4 Renderer::GenerateProjectionMatrix()
{
	glm::mat4 projMatrix = glm::perspective(glm::radians(m_camera->m_fov), (float)m_drawExtent.width / (float)m_drawExtent.height, m_fNearPlane, m_fFarPlane);
	projMatrix[1][1] *= -1;

	return projMatrix;
}

glm::mat4 Renderer::GenerateOrthographicMatrix()
{
	glm::mat4 orthoMatrix = glm::ortho(-1.0f, (m_camera->m_scale / m_drawExtent.width), (m_camera->m_scale / m_drawExtent.height), -1.0f, m_fNearPlane, m_fFarPlane); //need to readd scale and ratio is properly to screen size
	return orthoMatrix;
}

glm::mat4 Renderer::GenerateUIOrthographicMatrix()
{
	glm::mat4 orthoMatrix = glm::ortho(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, m_fFarPlane); //need to readd scale and ratio is properly to screen size
	return orthoMatrix;
}

glm::mat4 Renderer::GenerateViewMatrix()
{
	if (m_camera == nullptr)
		return glm::mat4(1.0f);

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Transform* camTransform = registry->GetComponent<Transform>(m_camera->m_camTransformIndex);
	if (camTransform == nullptr)
		return glm::mat4(1.0f);

	glm::mat4 viewMatrix = glm::lookAt(camTransform->m_position, camTransform->m_position + camTransform->m_rotation, glm::vec3(0.0f, 1.0f, 0.0f));
	return viewMatrix;
}

void Renderer::AddOnScreenObject(Renderable* comp)
{
	m_onScreenObjects.push_back(comp);
}

void Renderer::GetOnScreenVerticesAndTriangles(int& vertCount, int& triCount)
{
	for (int i = 0; i < m_onScreenObjects.size(); i++)
	{
		vertCount += m_onScreenObjects.at(i)->m_vertices->size();
		triCount += m_onScreenObjects.at(i)->m_indices->size();
	}

	triCount /= 3;
}

AllocatedBuffer Renderer::CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, std::string allocationName)
{
	VkBufferCreateInfo bufferInfo = { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.pNext = nullptr;
	bufferInfo.size = allocSize;

	bufferInfo.usage = usage;

	VmaAllocationCreateInfo vmaAllocInfo = {};
	vmaAllocInfo.usage = memoryUsage;
	vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
	AllocatedBuffer newBuffer;

	if (vmaCreateBuffer(m_allocator, &bufferInfo, &vmaAllocInfo, &newBuffer.m_buffer, &newBuffer.m_allocation, &newBuffer.m_info) != VK_SUCCESS)
		throw std::exception();

	newBuffer.m_allocation->SetName(m_allocator, allocationName.c_str());

	return newBuffer;
}

void Renderer::DestroyBuffer(const AllocatedBuffer& buffer)
{
	vmaDestroyBuffer(m_allocator, buffer.m_buffer, buffer.m_allocation);
}

GPUMeshBuffers Renderer::UploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices, std::string meshName)
{
	const size_t vertexBufferSize = vertices.size() * sizeof(Vertex);
	const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

	GPUMeshBuffers newSurface;

	newSurface.m_vertexBuffer = CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY, meshName+"Vertex");

	//Get buffer device address here.
	VkBufferDeviceAddressInfo deviceAddressInfo = { .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = newSurface.m_vertexBuffer.m_buffer };
	newSurface.m_vertexBufferAddress = vkGetBufferDeviceAddress(m_device, &deviceAddressInfo);

	newSurface.m_indexBuffer = CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, meshName+"Index");

	AllocatedBuffer staging = CreateBuffer(vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, meshName + "Staging");
	void* data = staging.m_allocation->GetMappedData();

	//Copy into start of memory block, vertices
	memcpy(data, vertices.data(), vertexBufferSize);
	//Copy into the memory block, where the vertex buffer data ends.
	memcpy((char*)data + vertexBufferSize, indices.data(), indexBufferSize);

	ImmediateSubmit([&](VkCommandBuffer cmd) {
		VkBufferCopy vertexCopy{ 0 };
	vertexCopy.dstOffset = 0;
	vertexCopy.srcOffset = 0;
	vertexCopy.size = vertexBufferSize;

	vkCmdCopyBuffer(cmd, staging.m_buffer, newSurface.m_vertexBuffer.m_buffer, 1, &vertexCopy);

	VkBufferCopy indexCopy{ 0 };
	indexCopy.dstOffset = 0;
	indexCopy.srcOffset = vertexBufferSize;
	indexCopy.size = indexBufferSize;

	vkCmdCopyBuffer(cmd, staging.m_buffer, newSurface.m_indexBuffer.m_buffer, 1, &indexCopy);
		});

	m_mainDeletionQueue.push_function([=]()
	{
		DestroyBuffer(newSurface.m_vertexBuffer);
		DestroyBuffer(newSurface.m_indexBuffer);
	});

	DestroyBuffer(staging);
	return newSurface;
}

AllocatedImage Renderer::CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped, std::string imageName)
{
	AllocatedImage newImage;
	newImage.m_imageFormat = format;
	newImage.m_imageExtent = size;

	VkImageCreateInfo img_info = vkinit::ImageCreateInfo(format, usage, size);
	if (mipmapped) {
		img_info.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(size.width, size.height)))) + 1;
	}

	// always allocate images on dedicated GPU memory
	VmaAllocationCreateInfo allocinfo = {};
	allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// allocate and create the image
	if (vmaCreateImage(m_allocator, &img_info, &allocinfo, &newImage.m_image, &newImage.m_allocation, nullptr) != VK_SUCCESS)
		LogFatalError("Failed to allocate image.");

	// if the format is a depth format, we will need to have it use the correct
	// aspect flag
	VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
	if (format == VK_FORMAT_D32_SFLOAT) {
		aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
	}

	// build a image-view for the image
	VkImageViewCreateInfo view_info = vkinit::ImageViewCreateInfo(format, newImage.m_image, aspectFlag);
	view_info.subresourceRange.levelCount = img_info.mipLevels;

	if (vkCreateImageView(m_device, &view_info, nullptr, &newImage.m_imageView) != VK_SUCCESS)
		LogFatalError("Failed to create image view.");

	newImage.m_allocation->SetName(m_allocator, imageName.c_str());
	return newImage;
}

AllocatedImage Renderer::CreateImage(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped, std::string imageName)
{
	size_t data_size = size.depth * size.width * size.height * 4;
	AllocatedBuffer uploadbuffer = CreateBuffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, imageName + "ImageUploadBuffer");

	memcpy(uploadbuffer.m_info.pMappedData, data, data_size);

	AllocatedImage new_image = CreateImage(size, format, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, mipmapped, imageName);

	ImmediateSubmit([&](VkCommandBuffer cmd)
	{
		vkutil::TransitionImage(cmd, new_image.m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset = 0;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageExtent = size;

		// copy the buffer into the image
		vkCmdCopyBufferToImage(cmd, uploadbuffer.m_buffer, new_image.m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
			&copyRegion);

		if (mipmapped)
		{
			vkutil::GenerateMipmaps(cmd, new_image.m_image, VkExtent2D{ new_image.m_imageExtent.width,new_image.m_imageExtent.height });
		}
		else
		{
			vkutil::TransitionImage(cmd, new_image.m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	});

	DestroyBuffer(uploadbuffer);
	return new_image;
}

void Renderer::DestroyImage(AllocatedImage& img)
{
	vkDestroyImageView(m_device, img.m_imageView, nullptr);
	vmaDestroyImage(m_allocator, img.m_image, img.m_allocation);
}


void Renderer::ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function)
{
	if (vkResetFences(m_device, 1, &m_immediateFence) != VK_SUCCESS)
		LogFatalError("Failed to reset immediate fence.");
	if (vkResetCommandBuffer(m_immediateCommandBuffer, 0) != VK_SUCCESS)
		LogFatalError("Failed to reset immediate command buffer.");

	VkCommandBuffer cmd = m_immediateCommandBuffer;

	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	if (vkBeginCommandBuffer(cmd, &cmdBeginInfo) != VK_SUCCESS)
		LogFatalError("Failed to begin immediate command buffer.");

	function(cmd);

	if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
		LogFatalError("Failed to end immediate command buffer.");

	VkCommandBufferSubmitInfo cmdInfo = vkinit::CommandBufferSubmitInfo(cmd);
	VkSubmitInfo2 submit = vkinit::SubmitInfo(&cmdInfo, nullptr, nullptr);

	if (vkQueueSubmit2(m_graphicsQueue, 1, &submit, m_immediateFence) != -VK_SUCCESS)
		LogFatalError("Failed to submit immediate queue.");

	if (vkWaitForFences(m_device, 1, &m_immediateFence, true, 99999999) != VK_SUCCESS)
		LogFatalError("Failed to wait for immediate fence.");
}