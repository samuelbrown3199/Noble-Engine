#pragma once
#ifndef RENDERER_H_
#define RENDERER_H_

#include <iostream>
#include <exception>
#include <optional>

#include <SDL/SDL.h>
#include <SDL/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "GraphicsPipeline.h"
#include "..\Systems\Camera.h"

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class Renderer
{
private:
	static SDL_Window* m_gameWindow;

	uint32_t imageIndex;
	uint32_t m_iCurrentFrame = 0;
	const int MAX_FRAMES_IN_FLIGHT = 2;

	static VkInstance m_vulkanInstance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	static VkPhysicalDevice m_physicalDevice;
	static VkDevice m_device;
	VkSurfaceKHR m_surface;
	VkSwapchainKHR m_swapChain;
	static VkFormat m_swapChainImageFormat;
	static VkExtent2D m_swapChainExtent;
	std::vector<VkImageView> m_vSwapChainImageViews;
	std::vector<VkFramebuffer> m_vSwapchainFramebuffers;
	VkCommandPool m_commandPool;
	static VkCommandBuffer m_currentCommandBuffer;
	std::vector<VkCommandBuffer> m_vCommandBuffers;

	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

	std::vector<VkImage> m_vSwapChainImages;
	GraphicsPipeline* m_graphicsPipeline;

	std::vector <VkSemaphore> m_vImageAvailableSemaphores;
	std::vector <VkSemaphore> m_vRenderFinishedSemaphores;
	std::vector<VkFence> m_vInFlightFences;

	static int m_iScreenWidth, m_iScreenHeight;
	static float m_fNearPlane, m_fFarPlane;
	static float m_fScale;

	static Camera* m_camera;

	static float m_fFov;
	static const float m_fMaxScale, m_fMinScale;

	static bool m_bProjectionRendering;

	const std::vector<const char*> m_vValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	const std::vector<const char*> m_vDeviceExtensions = 
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

#ifdef NDEBUG
	const bool m_bEnableValidationLayers = false;
#else
	const bool m_bEnableValidationLayers = true;
#endif

	static glm::mat4 GenerateProjectionMatrix();
	static glm::mat4 GenerateOrthographicMatrix();

	void InitializeVulkan();
	void CleanupVulkan();
	void CreateInstance();
	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();

	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void SetupDebugMessenger();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	void CreateSurface();

	bool IsDeviceSuitable(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	void PickPhysicalDevice();
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	void CreateLogicalDevice();

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void CreateSwapChain();

	void CreateImageViews();
	void CreateGraphicsPipeline();
	void CreateFrameBuffers();
	void CreateCommandPool();
	void CreateCommandBuffer();
	void StartRecordingCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void EndRecordingCommandBuffer(VkCommandBuffer commandBuffer);
	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	void CreateSyncObjects();

	void CleanupSwapchain();
	void RecreateSwapchain();

public:

	Renderer(const std::string _windowName);
	~Renderer();

	static void UpdateScreenSize();
	static void UpdateScreenSize(const int& _height, const int& _width);
	static SDL_Window* GetWindow() { return m_gameWindow; }

	// 0 Windowed, 1 Fullscreen, 2 Borderless Windowed
	static void SetWindowFullScreen(const int& _mode);
	static void SetVSyncMode(const int& _mode);

	//Adds the amount onto scale. The larger the scale the larger the world.
	static void AdjustScale(const float& _amount);

	static void SetCamera(Camera* cam) { m_camera = cam; }
	static Camera* GetCamera() { return m_camera; };
	static float GetScale() { return m_fScale; };
	static glm::vec2 GetScreenSize() { return glm::vec2(m_iScreenWidth, m_iScreenHeight); };

	static glm::mat4 GenerateProjMatrix();
	static glm::mat4 GenerateUIOrthographicMatrix();
	static glm::mat4 GenerateViewMatrix();

	static std::string GetWindowTitle();
	static void UpdateWindowTitle(const std::string& _windowTitle) { SDL_SetWindowTitle(m_gameWindow, _windowTitle.c_str()); };

	static void SetFov(float value) { m_fFov = value; }
	static float GetFov() { return m_fFov; }


	static VkInstance GetVulkanInstance() { return m_vulkanInstance; }
	static VkPhysicalDevice GetPhysicalDevice() { return m_physicalDevice; }
	static VkDevice GetLogicalDevice() { return m_device; }
	static VkFormat GetSwapchainImageFormat() { return m_swapChainImageFormat; }
	static VkExtent2D GetSwapchainExtent() { return m_swapChainExtent; }
	static VkCommandBuffer GetCurrentCommandBuffer() { return m_currentCommandBuffer; }

	void StartDrawFrame();
	void EndDrawFrame();

	void DrawFrame();
};

#endif