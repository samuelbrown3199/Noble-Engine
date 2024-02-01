#pragma once

#include "VulkanTypes.h"

struct DescriptorLayoutBuilder
{
    std::vector<VkDescriptorSetLayoutBinding> m_bindings;

    void AddBinding(uint32_t binding, VkDescriptorType type);
    void Clear();
    VkDescriptorSetLayout Build(VkDevice device, VkShaderStageFlags shaderStages);
};

struct DescriptorAllocator
{
    struct PoolSizeRatio
    {
        VkDescriptorType m_type;
        float m_ratio;
    };

    VkDescriptorPool m_pool;

    void InitializePool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
    void ClearDescriptors(VkDevice device);
    void DestroyPool(VkDevice device);

    VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout);
};