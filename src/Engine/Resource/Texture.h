#pragma once
#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Resource.h"

#include<GL/glew.h>

/**
*Stores a texture for use in the engine.
*/
struct Texture : public Resource
{
    int m_iWidth = 0;
    int m_iHeight = 0;

    GLuint m_iTextureID;

    uint32_t m_iMipLevels;

    enum FilterMode
    {
        Point,
        Linear
    };

    FilterMode m_filterMode;

    void OnLoad() override;
    void OnUnload() override;

    virtual void AddResource(std::string path) override;
    virtual std::vector<std::shared_ptr<Resource>> GetResourcesOfType() override;

    virtual void SetResourceToDefaults(std::shared_ptr<Resource> res) override;

    Texture();
    ~Texture();

    virtual void DoResourceInterface() override;
    nlohmann::json AddToDatabase() override;
    std::shared_ptr<Resource> LoadFromJson(const std::string& path, const nlohmann::json& data) override;
    void SetDefaults(const nlohmann::json& data) override;
};

#endif