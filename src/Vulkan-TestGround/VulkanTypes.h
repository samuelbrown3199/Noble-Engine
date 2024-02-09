#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <span>
#include <array>
#include <functional>
#include <deque>
#include <iostream>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vma/vk_mem_alloc.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

struct AllocatedImage
{
    VkImage m_image;
    VkImageView m_imageView;
    VmaAllocation m_allocation;
    VkExtent3D m_imageExtent;
    VkFormat m_imageFormat;
};

struct AllocatedBuffer
{
    VkBuffer m_buffer;
    VmaAllocation m_allocation;
    VmaAllocationInfo m_info;
};

struct Vertex
{
    glm::vec3 m_position;
    float m_uvX;
    glm::vec3 m_normal;
    float m_uvY;
    glm::vec4 m_colour;
};

struct GPUMeshBuffers
{
    AllocatedBuffer m_indexBuffer;
    AllocatedBuffer m_vertexBuffer;
    VkDeviceAddress m_vertexBufferAddress;
};

struct GPUDrawPushConstants
{
    glm::mat4 m_worldMatrix;
    VkDeviceAddress m_vertexBuffer;
};