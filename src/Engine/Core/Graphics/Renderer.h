#pragma once

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

	VkDescriptorSet m_sceneDescriptor;
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
	SDL_Window* m_gameWindow = nullptr;

	uint32_t m_iCurrentFrame = 0;
	const static int MAX_FRAMES_IN_FLIGHT = 2;
	VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	FrameData m_frames[MAX_FRAMES_IN_FLIGHT];

	bool m_bVsync = true;
	glm::vec3 m_clearColour = glm::vec3(0.0f, 0.0f, 0.0f);;

	VkInstance m_vulkanInstance;
	ImGuiContext* m_imguiContext;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;
	VkSurfaceKHR m_surface;

	//draw resources
	AllocatedImage m_drawImage;
	AllocatedImage m_depthImage;
	VkExtent2D m_drawExtent;
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

	VkQueue m_graphicsQueue;
	uint32_t m_graphicsQueueFamily;

	bool m_bDrawToWindow = false;
	VkDescriptorSet* m_drawWindowSet = nullptr;

	DeletionQueue m_mainDeletionQueue;
	VmaAllocator m_allocator;

	VkFence m_immediateFence;
	VkCommandBuffer m_immediateCommandBuffer;
	VkCommandPool m_immediateCommandPool;

	int m_iScreenWidth = 500, m_iScreenHeight = 500;
	float m_fNearPlane = 0.1f, m_fFarPlane = 1000.0f;
	const float m_fMaxScale = 1000, m_fMinScale = 3;

	Camera* m_camera = nullptr;

	std::vector<Renderable*> m_onScreenObjects;
	int m_iRenderableCount = 0;

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
	void RegisterDescriptors();

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	void CreateSwapchain(uint32_t width, uint32_t height);
	void DestroySwapchain();
	void RecreateSwapchain();

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	void InitializeImgui();

	void ResetForNextFrame();
	void DrawFrame();
	void DrawBackground(VkCommandBuffer cmd);
	void DrawGeometry(VkCommandBuffer cmd);
	void DrawImGui(VkCommandBuffer cmd, VkImage targetImage, VkImageView targetImageView);

	void InitializeDefaultData();

	GPUMeshBuffers GetSpriteQuadBuffer() { return m_spriteQuad; }

public:

	//to be removed/reworked
	VkDescriptorSetLayout m_singleImageDescriptorLayout;
	VkDescriptorSetLayout m_gpuSceneDataDescriptorLayout;
	//end of to be removed...

	GPUSceneData m_sceneData;

	Renderer(const std::string _windowName);
	~Renderer();

	void WaitForRenderingToFinish();

	FrameData& GetCurrentFrame() { return m_frames[m_iCurrentFrame % MAX_FRAMES_IN_FLIGHT]; }

	void AddOnScreenObject(Renderable* comp);
	std::vector<Renderable*>* GetOnScreenObjects() { return &m_onScreenObjects; }

	void CheckScreenSizeForUpdate();
	void UpdateScreenSize(const int& _height, const int& _width);
	SDL_Window* GetWindow() { return m_gameWindow; }

	// 0 Windowed, 1 Fullscreen, 2 Borderless Windowed
	void SetWindowFullScreen(const int& _mode);
	void SetVSyncMode(const int& _mode);
	void SetRenderScale(const float& value);
	float GetRenderScale() { return m_fRenderScale; }

	void SetClearColour(const glm::vec3 colour) { m_clearColour = colour; }
	glm::vec3 GetClearColour() { return m_clearColour; }

	void SetCamera(Camera* cam) { m_camera = cam; }
	Camera* GetCamera() { return m_camera; };
	glm::vec2 GetScreenSize() { return glm::vec2(m_iScreenWidth, m_iScreenHeight); };

	glm::mat4 GenerateProjMatrix();
	glm::mat4 GenerateProjectionMatrix();
	glm::mat4 GenerateOrthographicMatrix();
	glm::mat4 GenerateUIOrthographicMatrix();
	glm::mat4 GenerateViewMatrix();

	std::string GetWindowTitle();
	void UpdateWindowTitle(const std::string& _windowTitle) { SDL_SetWindowTitle(m_gameWindow, _windowTitle.c_str()); };

	VkQueue GetGraphicsQueue() { return m_graphicsQueue; }

	VkInstance GetVulkanInstance() { return m_vulkanInstance; }
	VkPhysicalDevice GetPhysicalDevice() { return m_physicalDevice; }
	VkDevice GetLogicalDevice() { return m_device; }

	VmaAllocator GetAllocator() { return m_allocator; }

	VkSampleCountFlagBits GetMSAALevel() { return m_msaaSamples; }

	void IncrementRenderables() { m_iRenderableCount++; }
	int GetRenderableCount() { return m_iRenderableCount; }
	size_t GetOnScreenRenderableCount() { return m_onScreenObjects.size(); }
	void GetOnScreenVerticesAndTriangles(int& vertCount, int& triCount);

	void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

	//Functions below here likely to be moved/removed at some point
	AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, std::string allocationName);
	void DestroyBuffer(const AllocatedBuffer& buffer);
	GPUMeshBuffers UploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices, std::string meshName);

	AllocatedImage CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped, std::string imageName);
	AllocatedImage CreateImage(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped, std::string imageName);
	void DestroyImage(AllocatedImage& img);

	AllocatedImage GetCheckerboardErrorTexture() { return m_errorCheckerboardImage; }
	VkSampler GetDefaultSampler() { return m_defaultSamplerNearest; }

	VkFormat GetDrawImageFormat() { return m_drawImage.m_imageFormat; }
	VkFormat GetDepthImageFormat() { return m_depthImage.m_imageFormat; }

	ImGuiContext* GetImguiContext() { return m_imguiContext; }
	void SetDrawWindowSet(VkDescriptorSet* set);
	void SetDrawToWindow(bool value) { m_bDrawToWindow = value; }
};