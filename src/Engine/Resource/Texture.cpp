#include "Texture.h"
#include "../Useful.h"
#include "../Core/Logger.h"
#include "../Core/Graphics/Renderer.h"

#include "../Core/ResourceManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Texture::OnLoad()
{
    if (m_bIsLoaded)
        return;

    stbi_uc* pixels = stbi_load(m_sResourcePath.c_str(), &m_iWidth, &m_iHeight, NULL, 4);
    if (!pixels)
    {
        Logger::LogError(FormatString("Failed to load texture file %s", m_sResourcePath.c_str()), 1);
        throw std::exception();
    }

    //create and bind a texture
    glGenTextures(1, &m_iTextureID);
    if (!m_iTextureID)
    {
        Logger::LogError("Failed to create texture ID", 1);
        throw std::exception();
    }
    glBindTexture(GL_TEXTURE_2D, m_iTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLuint filterMode = 0;
    switch (m_filterMode)
    {
    case Point:
        filterMode = GL_NEAREST;
        break;
    case Linear:
        filterMode = GL_LINEAR;
        break;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //upload the image data to the bound texture unit in the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    //free the loaded data because we now have a copy on the GPU
    free(pixels);
    //generate mipmap so the texture can be mapped correctly
    glGenerateMipmap(GL_TEXTURE_2D);
    //unbind the texture becase we are done operating on it
    glBindTexture(GL_TEXTURE_2D, 0);
    m_bIsLoaded = true;
}

void Texture::OnUnload()
{
    if (!m_bIsLoaded)
        return;
    

    glDeleteTextures(1, &m_iTextureID);
    m_bIsLoaded = false;
}

void Texture::AddResource(std::string path)
{
    ResourceManager::AddNewResource<Texture>(path);
}

std::vector<std::shared_ptr<Resource>> Texture::GetResourcesOfType()
{
    return ResourceManager::GetAllResourcesOfType<Texture>();
}

void Texture::SetResourceToDefaults(std::shared_ptr<Resource> res)
{
    std::shared_ptr<Texture> dyRes = std::dynamic_pointer_cast<Texture>(res);
    dyRes->m_filterMode = m_filterMode;
}

Texture::Texture()
{
    m_resourceType = "Texture";
}

Texture::~Texture()
{
    if (m_bIsLoaded)
    {
        OnUnload();
    }
}

void Texture::DoResourceInterface()
{
    ImGui::Text(m_sLocalPath.c_str());
    ImGui::Text(m_resourceType.c_str());

    if (m_bIsLoaded)
    {
        ImGui::Text("Texture is currently in use and can't be modified.");
        return;
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    const char* items[] = { "Nearest", "Linear" };
    int item = m_filterMode;
    ImGui::Combo("Filter Mode", &item, items, IM_ARRAYSIZE(items));

    m_filterMode = (FilterMode)item;
}

nlohmann::json Texture::AddToDatabase()
{
    nlohmann::json data;

    data["Filter"] = m_filterMode;

    return data;
}

std::shared_ptr<Resource> Texture::LoadFromJson(const std::string& path, const nlohmann::json& data)
{
    std::shared_ptr<Texture> res = std::make_shared<Texture>();

    res->m_sLocalPath = path;
    res->m_sResourcePath = GetGameFolder() + path;
    res->m_filterMode = data["Filter"];

    return res;
}

void Texture::SetDefaults(const nlohmann::json& data)
{
    m_filterMode = data["Filter"];
}