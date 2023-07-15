#pragma once
#ifndef RENDERER_H_
#define RENDERER_H_

#include <iostream>
#include <exception>
#include <optional>

#include <SDL/SDL.h>
#include <SDL/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/hash.hpp>

#include "GraphicsPipeline.h"
#include "..\..\Systems\Camera.h"

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	bool operator==(const Vertex& other) const 
	{
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}

	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		//Vertex info here.
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		//Color info here.
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}
};

namespace std
{
	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

struct UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

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

struct Texture;
struct Model;

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
	static VkCommandPool m_commandPool;
	static VkCommandBuffer m_currentCommandBuffer;
	std::vector<VkCommandBuffer> m_vCommandBuffers;

	//Consider how these can be put into seperate objects, maybe one for model loading later..
	static VkBuffer m_vertexBuffer;
	VkDeviceMemory m_vertexBufferMemory;

	static VkBuffer m_indexBuffer;
	VkDeviceMemory m_indexBufferMemory;

	std::vector<VkBuffer> m_uniformBuffers;
	std::vector<VkDeviceMemory> m_uniformBuffersMemory;
	std::vector<void*> m_uniformBuffersMapped;

	VkDescriptorPool m_descriptorPool;
	std::vector<VkDescriptorSet> m_descriptorSets;

	static VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

	std::vector<VkImage> m_vSwapChainImages;
	GraphicsPipeline* m_graphicsPipeline;

	std::vector <VkSemaphore> m_vImageAvailableSemaphores;
	std::vector <VkSemaphore> m_vRenderFinishedSemaphores;
	std::vector<VkFence> m_vInFlightFences;

	std::shared_ptr<Texture> m_texture;
	std::shared_ptr<Model> m_model;

	VkImage m_depthImage;
	VkDeviceMemory m_depthImageMemory;
	VkImageView m_depthImageView;

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

	static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	void CreateDepthResources();

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

	static VkFormat FindDepthFormat();
	static bool HasStencilComponent(VkFormat format);

	//-------------------------------BUFFER STUFFS-------------------------------------

	static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	static void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	static void CreateVertexBuffer();
	static void CreateIndexBuffer();
	void CreateUniformBuffers();

	static VkCommandBuffer BeginSingleTimeCommand();
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	static void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

	void UpdateUniformBuffer(uint32_t currentImage);

	void CreateDescriptorPool();
	void CreateDescriptorSets();

	//---------------------------------------------------------------------------------

	//-------------------------------FUNCTIONS FOR PROTOTYPING-------------------------------------

	void DrawFrame();

	void LoadModel();
	void CreateTextureImage();

	static VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	static VkBuffer GetIndexBuffer() { return m_indexBuffer; }
	static VkBuffer GetVertexBuffer() { return m_vertexBuffer; }
};

#endif