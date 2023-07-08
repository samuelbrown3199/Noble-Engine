#include "Renderer.h"

#include <set>

#include "Logger.h"
#include "../Useful.h"

SDL_Window* Renderer::m_gameWindow;

int Renderer::m_iScreenWidth = 500;
int Renderer::m_iScreenHeight = 500;
float Renderer::m_fNearPlane = 0.1f;
float Renderer::m_fFarPlane = 1000.0f;
float Renderer::m_fScale = 20;

float Renderer::m_fFov = 90.0f;
const float Renderer::m_fMaxScale = 1000;
const float Renderer::m_fMinScale = 3;

Camera* Renderer::m_camera = nullptr;

bool Renderer::m_bProjectionRendering = true;

VkDevice Renderer::m_device;
VkFormat Renderer::m_swapChainImageFormat;
VkExtent2D Renderer::m_swapChainExtent;

//---------- public functions ---------

Renderer::Renderer(const std::string _windowName)
{
	Logger::LogInformation("Creating engine renderer.");

	m_gameWindow = SDL_CreateWindow(_windowName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_iScreenWidth, m_iScreenHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
	if (!m_gameWindow)
	{
		Logger::LogInformation("Failed to create game window");
		std::cout << "Failed to create game window " << SDL_GetError() << std::endl;
		throw std::exception();
	}
	Logger::LogInformation("Created game window.");

	InitializeVulkan();
}

Renderer::~Renderer()
{
	delete m_camera;

	CleanupVulkan();

	SDL_DestroyWindow(m_gameWindow);
}

void Renderer::InitializeVulkan()
{
	Logger::LogInformation("Initializing Vulkan");
	CreateInstance();
	SetupDebugMessenger();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();
	CreateImageViews();
	CreateGraphicsPipeline();

	Logger::LogInformation("Initialized Vulkan");
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

void Renderer::CleanupVulkan()
{
	delete m_graphicsPipeline;
	for (auto imageView : m_vSwapChainImageViews)
	{
		vkDestroyImageView(m_device, imageView, nullptr);
	}
	vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
	vkDestroyDevice(m_device, nullptr);
	if (m_bEnableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_vulkanInstance, m_debugMessenger, nullptr);
	}
	vkDestroySurfaceKHR(m_vulkanInstance, m_surface, nullptr);
	vkDestroyInstance(m_vulkanInstance, nullptr);
}

void Renderer::CreateInstance()
{
	if (m_bEnableValidationLayers && !CheckValidationLayerSupport())
	{
		Logger::LogError("Validation Layers requested, but not available.", 2);
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = GetWindowTitle().c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Noble Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	auto extensions = GetRequiredExtensions();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	if (m_bEnableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_vValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_vValidationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &m_vulkanInstance);
	if (result != VK_SUCCESS)
	{
		Logger::LogError(FormatString("Failed to create Vulkan Instance, &d", result), 2);
	}
}

bool Renderer::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : m_vValidationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

std::vector<const char*> Renderer::GetRequiredExtensions()
{
	uint32_t sdlExtensionCount = 0;
	SDL_Vulkan_GetInstanceExtensions(m_gameWindow, &sdlExtensionCount, NULL);
	const char** sdlExtensions = (const char**)malloc(sizeof(const char*) * sdlExtensionCount);
	SDL_bool result = SDL_Vulkan_GetInstanceExtensions(m_gameWindow, &sdlExtensionCount, sdlExtensions);
	if (result == SDL_FALSE)
	{
		Logger::LogError(FormatString("Failed to create SDL Instance Extensions, &s", SDL_GetError()), 2);
	}

	std::vector<const char*> extensions(sdlExtensions, sdlExtensions + sdlExtensionCount);

	if (m_bEnableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Renderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr; // Optional
}

void Renderer::SetupDebugMessenger()
{
	if (!m_bEnableValidationLayers)
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	PopulateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(m_vulkanInstance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
	{
		Logger::LogError("Failed to create Debug Messenger.", 2);
	}
}



VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	Logger::LogInformation(FormatString("Validation Layer: %s", pCallbackData->pMessage));

	return VK_FALSE;
}



void Renderer::CreateSurface()
{
	if (!SDL_Vulkan_CreateSurface(m_gameWindow, m_vulkanInstance, &m_surface))
	{
		Logger::LogError("Failed to create Vulkan Surface.", 2);
	}
}



bool Renderer::IsDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);
	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.IsComplete() && extensionsSupported && swapChainAdequate;
}

bool Renderer::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(m_vDeviceExtensions.begin(), m_vDeviceExtensions.end());
	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void Renderer::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_vulkanInstance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		Logger::LogError("Failed to find GPU with Vulkan support.", 2);
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_vulkanInstance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			m_physicalDevice = device;
			break;
		}
	}

	if (m_physicalDevice == VK_NULL_HANDLE)
	{
		Logger::LogError("Failed to find suitable GPU.", 2);
	}
}

QueueFamilyIndices Renderer::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;
	//Logic to find queue family indices.
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}	
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.IsComplete())
			break;

		i++;
	}

	return indices;
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
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Renderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		SDL_Vulkan_GetDrawableSize(m_gameWindow, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void Renderer::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
	{
		Logger::LogError("Failed to create swap chain.", 2);
	}

	vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
	m_vSwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_vSwapChainImages.data());

	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;
}

void Renderer::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = 1;

	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_vDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_vDeviceExtensions.data();

	if (m_bEnableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_vValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_vValidationLayers.data();
	}
	else 
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) 
	{
		Logger::LogError("Failed to create logical device.", 2);
	}

	vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
}

void Renderer::CreateImageViews()
{
	m_vSwapChainImageViews.resize(m_vSwapChainImages.size());

	for (size_t i = 0; i < m_vSwapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_vSwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_device, &createInfo, nullptr, &m_vSwapChainImageViews[i]) != VK_SUCCESS)
		{
			Logger::LogError("Failed to create image views.", 2);
		}
	}
}

void Renderer::CreateGraphicsPipeline()
{
	m_graphicsPipeline = new GraphicsPipeline();
}

void Renderer::AdjustScale(const float& _amount)
{ 
	m_fScale += _amount;
	if (m_fScale > m_fMaxScale)
		m_fScale = m_fMaxScale;
	else if (m_fScale < m_fMinScale)
		m_fScale = m_fMinScale;
}

void Renderer::ClearBuffer()
{

}
void Renderer::SwapGraphicsBuffer()
{

}
void Renderer::UpdateScreenSize()
{
	int newWidth, newHeight;
	SDL_GetWindowSize(m_gameWindow, &newWidth, &newHeight);
	if (newWidth != m_iScreenWidth || newHeight != m_iScreenHeight)
	{
		m_iScreenWidth = newWidth;
		m_iScreenHeight = newHeight;

		Logger::LogInformation(FormatString("Resized game window to %dx%d.", m_iScreenWidth, m_iScreenHeight));
	}
}

void Renderer::UpdateScreenSize(const int& _height, const int& _width)
{
	int newWidth = _width, newHeight = _height;
	if (newWidth != m_iScreenWidth || newHeight != m_iScreenHeight)
	{
		m_iScreenWidth = newWidth;
		m_iScreenHeight = newHeight;
		SDL_SetWindowSize(m_gameWindow, m_iScreenWidth, m_iScreenHeight);
		Logger::LogInformation(FormatString("Resized game window to %dx%d.", m_iScreenWidth, m_iScreenHeight));
	}
}

void Renderer::SetWindowFullScreen(const int& _mode)
{
	Logger::LogInformation(FormatString("Changing window fullscreen mode to %d", _mode));
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

	Renderer::UpdateScreenSize();
}

std::string Renderer::GetWindowTitle()
{
	std::string name(SDL_GetWindowTitle(m_gameWindow));
	return name;
}

// 0 No VSync, 1 VSync, 2 Adaptive VSync
void Renderer::SetVSyncMode(const int& _mode)
{
	Logger::LogInformation(FormatString("Changing VSync mode to %d", _mode));
}

glm::mat4 Renderer::GenerateProjMatrix()
{
	if (m_bProjectionRendering)
		return GenerateProjectionMatrix();

	return GenerateOrthographicMatrix();
}

glm::mat4 Renderer::GenerateProjectionMatrix()
{
	glm::mat4 projMatrix = glm::perspective(glm::radians(m_fFov), (float)m_iScreenWidth / (float)m_iScreenHeight, m_fNearPlane, m_fFarPlane);
	return projMatrix;
}

glm::mat4 Renderer::GenerateOrthographicMatrix()
{
	glm::mat4 orthoMatrix = glm::ortho(0.0f, (float)m_iScreenWidth / m_fScale, (float)m_iScreenHeight / m_fScale, 0.0f, 0.0f, m_fFarPlane);
	return orthoMatrix;
}

glm::mat4 Renderer::GenerateUIOrthographicMatrix()
{
	glm::mat4 orthoMatrix = glm::ortho(0.0f, (float)m_iScreenWidth, (float)m_iScreenHeight, 0.0f, 0.0f, m_fFarPlane);
	return orthoMatrix;
}

glm::mat4 Renderer::GenerateViewMatrix()
{
	glm::mat4 viewMatrix = glm::lookAt(m_camera->m_camTransform->m_position, m_camera->m_camTransform->m_position + m_camera->m_camTransform->m_rotation, glm::vec3(0.0f, 1.0f, 0.0f));
	return viewMatrix;
}