// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

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
};

constexpr unsigned int FRAME_OVERLAP = 2;

class Renderer {
public:

	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;
	VkSurfaceKHR m_surface;

	VkSwapchainKHR m_swapchain;
	VkFormat m_swapchainImageFormat;
	std::vector<VkImage> m_swapchainImages;
	std::vector<VkImageView> m_swapchainImageViews;
	VkExtent2D m_swapchainExtent;

	FrameData m_frames[FRAME_OVERLAP];

	VkQueue m_graphicsQueue;
	uint32_t m_graphicsQueueFamily;

	DeletionQueue m_mainDeletionQueue;

	VmaAllocator m_allocator;

	//draw resources
	AllocatedImage m_drawImage;
	VkExtent2D m_drawExtent;

	DescriptorAllocator m_globalDescriptorAllocator;

	VkDescriptorSet m_drawImageDescriptors;
	VkDescriptorSetLayout m_drawImageDescriptorLayout;

	VkPipeline m_gradientPipeline;
	VkPipelineLayout m_gradientPipelineLayout;

	bool m_bIsInitialized{ false };
	bool m_bUseValidationLayer = true;
	int m_iFrameNumber{ 0 };
	bool m_bStopRendering{ false };
	VkExtent2D m_windowExtent{ 1700 , 900 };

	struct SDL_Window* m_window{ nullptr };
	FrameData& GetCurrentFrame() { return m_frames[m_iFrameNumber % FRAME_OVERLAP]; }

	static Renderer& Get();

	//initializes everything in the engine
	void init();

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();

	//run main loop
	void run();

private:

	void CreateSwapchain(uint32_t width, uint32_t height);
	void DestroySwapchain();

	void DrawBackground(VkCommandBuffer cmd);

	void InitializeVulkan();
	void InitializeSwapchain();
	void InitializeCommands();
	void InitializeSyncStructures();
	void InitializeDescriptors();
	void InitializePipelines();
	void InitializeBackgroundPipelines();
};
