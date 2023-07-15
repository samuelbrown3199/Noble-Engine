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

    VkImage m_textureImage;
    VkDeviceMemory m_textureImageMemory;
    VkImageView m_textureImageView;
    VkSampler m_textureSampler;

    void OnLoad();

    //These functions here are probably worth moving out of the texture object at some point, they are helpful functions
    static void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    static void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    void CreateTextureImageView();
    void CreateTextureSampler();

    ~Texture();
};

#endif