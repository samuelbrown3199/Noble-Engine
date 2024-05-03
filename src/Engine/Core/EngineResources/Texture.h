#pragma once
#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Resource.h"
#include "../../Core/Graphics/VulkanTypes.h"

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

    AllocatedImage m_texture;
    VkSampler m_textureSampler;

    void OnLoad() override;
    void OnUnload() override;

    virtual void AddResource(std::string path) override;

    virtual void SetResourceToDefaults(std::shared_ptr<Resource> res) override;

    void CreateTextureSampler(VkFilter filter);

    Texture();
    ~Texture();

    virtual void DoResourceInterface() override;
    nlohmann::json AddToDatabase() override;
    std::shared_ptr<Resource> LoadFromJson(const std::string& path, const nlohmann::json& data) override;
    void SetDefaults(const nlohmann::json& data) override;
};

#endif