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
#include <glm/gtx/hash.hpp>

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
    glm::vec4 m_colour = { 1,1,1,1 };

    bool operator==(const Vertex& other) const
    {
        return m_position == other.m_position && m_normal == other.m_normal && m_uvX == other.m_uvX && m_uvY == other.m_uvY && m_colour == other.m_colour;
    }

    Vertex() {};
    Vertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal, glm::vec4 colour)
    {
        m_position = pos;
        m_normal = normal;
        m_colour = colour;

        m_uvX = uv.x;
        m_uvY = uv.y;
    }
};

namespace std
{
    template<> struct hash<Vertex>
    {
        size_t operator()(Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.m_position) ^
                (hash<glm::vec3>()(vertex.m_normal) << 1) >> 1 ^
                (hash<float>()(vertex.m_uvX) << 1) >> 1 ^
                (hash<float>()(vertex.m_uvY) << 1) >> 1 ^
                (hash<glm::vec3>()(vertex.m_colour) << 1)));
        }
    };
}

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