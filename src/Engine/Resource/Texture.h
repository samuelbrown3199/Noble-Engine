#pragma once
#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Resource.h"

#include<vulkan/vulkan.h>

/**
*Stores a texture for use in the engine.
*/
struct Texture : public Resource
{
    int m_iWidth = 0;
    int m_iHeight = 0;
    int m_iTexChannels = 0;

    uint32_t m_iMipLevels;
    VkFilter m_textureFilter = VK_FILTER_LINEAR;

    VkImage m_textureImage;
    VkDeviceMemory m_textureImageMemory;
    VkImageView m_textureImageView;
    VkSampler m_textureSampler;

    void OnLoad() override;
    void OnUnload() override;

    void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    //These functions here are probably worth moving out of the texture object at some point, they are helpful functions
    static void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    static void TransitionImageLayout(VkImage image, uint32_t mipLevels, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

    void CreateTextureImageView();
    void CreateTextureSampler(VkFilter filter);

    Texture();
    ~Texture();

    virtual void DoResourceInterface() override;
    nlohmann::json AddToDatabase() override;
    void LoadFromJson(std::string path, nlohmann::json data) override;
};

#endif