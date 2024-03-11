#pragma once
#ifndef RENDERER_H_
#define RENDERER_H_

#include <iostream>
#include <exception>
#include <optional>

#include <SDL/SDL.h>
#include <SDL/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include "GraphicsPipeline.h"
#include "GraphicsBuffer.h"
#include "..\EngineComponents\Camera.h"

#include "VulkanTypes.h"
#include "VulkanDescriptors.h"

struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void flush() {
		// reverse iterate the deletion queue to execute all the functions
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
			(*it)(); //call functors
		}

		deletors.clear();
	}
};

struct FrameData
{
	VkCommandPool m_commandPool;
	VkCommandBuffer m_mainCommandBuffer;

	VkSemaphore m_swapchainSemaphore, m_renderSemaphore;
	VkFence m_renderFence;

	DeletionQueue m_deletionQueue;
	DescriptorAllocatorGrowable m_frameDescriptors;
};

struct GPUSceneData 
{
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 viewproj;
	glm::vec4 ambientColor;
	glm::vec4 sunlightDirection;
	glm::vec4 sunlightColor;
};

struct Renderable;

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


struct Pipeline;
struct Texture;
struct Model;

class Renderer
{
	friend class Application;
	friend struct Sprite;

private:
	static SDL_Window* m_gameWindow;

	uint32_t imageIndex;
	static uint32_t m_iCurrentFrame;
	const static int MAX_FRAMES_IN_FLIGHT = 2;
	static VkSampleCountFlagBits m_msaaSamples;

	static bool m_bVsync;
	static glm::vec3 m_clearColour;

	static VkInstance m_vulkanInstance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	static VkPhysicalDevice m_physicalDevice;
	static VkDevice m_device;
	VkSurfaceKHR m_surface;

	//draw resources
	AllocatedImage m_drawImage;
	AllocatedImage m_depthImage;
	static VkExtent2D m_drawExtent;
	float m_fRenderScale = 1.0f;
	VkFilter m_drawFilter = VK_FILTER_LINEAR;

	DescriptorAllocator m_globalDescriptorAllocator;

	VkDescriptorSet m_drawImageDescriptors;
	VkDescriptorSetLayout m_drawImageDescriptorLayout;

	VkSwapchainKHR m_swapchain;
	VkFormat m_swapchainImageFormat;
	std::vector<VkImage> m_swapchainImages;
	std::vector<VkImageView> m_swapchainImageViews;
	VkExtent2D m_swapchainExtent;

	FrameData m_frames[MAX_FRAMES_IN_FLIGHT];

	static VkDescriptorPool m_descriptorPool;

	static VkQueue m_graphicsQueue;
	uint32_t m_graphicsQueueFamily;

	GPUSceneData m_sceneData;
	VkDescriptorSetLayout m_gpuSceneDataDescriptorLayout;

	//temp values here for future editor update.
	bool drawToWindow = false;
	VkDescriptorSet m_drawWindowSet = VK_NULL_HANDLE;

	DeletionQueue m_mainDeletionQueue;
	static VmaAllocator m_allocator;

	static VkFence m_immediateFence;
	static VkCommandBuffer m_immediateCommandBuffer;
	static VkCommandPool m_immediateCommandPool;

	static int m_iScreenWidth, m_iScreenHeight;
	static float m_fNearPlane, m_fFarPlane;
	static const float m_fMaxScale, m_fMinScale;

	static Camera* m_camera;

	static std::vector<Renderable*> m_onScreenObjects;
	static int m_iRenderableCount;

	//Default Data
	AllocatedImage m_errorCheckerboardImage;

	VkSampler m_defaultSamplerLinear;
	VkSampler m_defaultSamplerNearest;
	
	GPUMeshBuffers m_spriteQuad;

	//Default Data end

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

	void InitializeVulkan();
	void CleanupVulkan();
	void CreateInstance();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	void InitializeSwapchain();
	void InitializeCommands();
	void InitializeSyncStructures();
	void InitializeDescriptors();

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	void CreateSwapchain(uint32_t width, uint32_t height);
	void DestroySwapchain();
	void RecreateSwapchain();

	static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	void InitializeImgui();

	void ResetForNextFrame();
	void DrawFrame();
	void DrawBackground(VkCommandBuffer cmd);
	void DrawGeometry(VkCommandBuffer cmd);
	void DrawImGui(VkCommandBuffer cmd, VkImage targetImage, VkImageView targetImageView);

	void InitializePipelines();
	void InitializeMeshPipelines();

	void InitializeDefaultData();

	GPUMeshBuffers GetSpriteQuadBuffer() { return m_spriteQuad; }

public:

	//to be removed
	std::shared_ptr<Pipeline> m_meshPipeline;
	VkDescriptorSetLayout m_singleImageDescriptorLayout;
	//end of to be removed...

	Renderer(const std::string _windowName);
	~Renderer();

	FrameData& GetCurrentFrame() { return m_frames[m_iCurrentFrame % MAX_FRAMES_IN_FLIGHT]; }

	static void AddOnScreenObject(Renderable* comp);
	static std::vector<Renderable*>* GetOnScreenObjects() { return &m_onScreenObjects; }

	static void UpdateScreenSize();
	static void UpdateScreenSize(const int& _height, const int& _width);
	static SDL_Window* GetWindow() { return m_gameWindow; }

	// 0 Windowed, 1 Fullscreen, 2 Borderless Windowed
	void SetWindowFullScreen(const int& _mode);
	void SetVSyncMode(const int& _mode);
	void SetRenderScale(const float& value);
	float GetRenderScale() { return m_fRenderScale; }
	void SetDrawImageFilter(const int& val);

	static void SetClearColour(const glm::vec3 colour) { m_clearColour = colour; }
	static glm::vec3 GetClearColour() { return m_clearColour; }

	static void SetCamera(Camera* cam) { m_camera = cam; }
	static Camera* GetCamera() { return m_camera; };
	static glm::vec2 GetScreenSize() { return glm::vec2(m_iScreenWidth, m_iScreenHeight); };

	static glm::mat4 GenerateProjMatrix();
	static glm::mat4 GenerateProjectionMatrix();
	static glm::mat4 GenerateOrthographicMatrix();
	static glm::mat4 GenerateUIOrthographicMatrix();
	static glm::mat4 GenerateViewMatrix();

	static std::string GetWindowTitle();
	static void UpdateWindowTitle(const std::string& _windowTitle) { SDL_SetWindowTitle(m_gameWindow, _windowTitle.c_str()); };

	static VkQueue GetGraphicsQueue() { return m_graphicsQueue; }

	static VkInstance GetVulkanInstance() { return m_vulkanInstance; }
	static VkPhysicalDevice GetPhysicalDevice() { return m_physicalDevice; }
	static VkDevice GetLogicalDevice() { return m_device; }

	VmaAllocator GetAllocator() { return m_allocator; }

	static VkSampleCountFlagBits GetMSAALevel() { return m_msaaSamples; }
	static VkDescriptorPool GetDescriptorPool() { return m_descriptorPool; }

	static void IncrementRenderables() { m_iRenderableCount++; }
	static int GetRenderableCount() { return m_iRenderableCount; }
	static size_t GetOnScreenRenderableCount() { return m_onScreenObjects.size(); }
	static void GetOnScreenVerticesAndTriangles(int& vertCount, int& triCount);

	static void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

	//Functions below here likely to be moved/removed at some point
	static AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
	static void DestroyBuffer(const AllocatedBuffer& buffer);
	static GPUMeshBuffers UploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);

	AllocatedImage CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
	AllocatedImage CreateImage(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
	void DestroyImage(const AllocatedImage& img);

	AllocatedImage GetCheckerboardErrorTexture() { return m_errorCheckerboardImage; }
	VkSampler GetDefaultSampler() { return m_defaultSamplerNearest; }

	VkFormat GetDrawImageFormat() { return m_drawImage.m_imageFormat; }
	VkFormat GetDepthImageFormat() { return m_depthImage.m_imageFormat; }

	//-------------------------------FUNCTIONS FOR PROTOTYPING-------------------------------------

	static VkImageView CreateImageView(VkImage image, uint32_t mipLevels, VkFormat format, VkImageAspectFlags aspectFlags);

};

#endif