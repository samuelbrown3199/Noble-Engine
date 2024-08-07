#include "Texture.h"
#include "../../Useful.h"
#include "../../Core/Logger.h"
#include "../../Core/Application.h"
#include "../../Core/Graphics/Renderer.h"

#include "../../Core/Graphics/VulkanImages.h"

#include "../../Core/ResourceManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Texture::OnLoad()
{
    if (m_bIsLoaded)
        return;

    Resource::OnLoad();

    stbi_uc* pixels = stbi_load(m_sResourcePath.c_str(), &m_iWidth, &m_iHeight, &m_iTexChannels, STBI_rgb_alpha);
    if(!pixels)
    {
        LogFatalError("Failed to load texture image.");
    }

    Renderer* renderer = Application::GetApplication()->GetRenderer();

    m_texture = renderer->CreateImage(pixels, VkExtent3D(m_iWidth, m_iHeight, 1), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, true, m_sLocalPath);
    CreateTextureSampler((VkFilter)m_textureFilter);

    free(pixels);

    m_bIsLoaded = true;
}

void Texture::OnUnload()
{
    if (!m_bIsLoaded)
        return;

    LogTrace("Unloading texture: " + m_sLocalPath);

    Renderer* renderer = Application::GetApplication()->GetRenderer();
    vkDestroySampler(renderer->GetLogicalDevice(), m_textureSampler, nullptr);
    renderer->DestroyImage(&m_texture);

    m_bIsLoaded = false;
}

void Texture::AddResource(std::string path)
{
    ResourceManager* resourceManager = Application::GetApplication()->GetResourceManager();
    resourceManager->AddNewResource<Texture>(path);
}

void Texture::SetResourceToDefaults(std::shared_ptr<Resource> res)
{
    std::shared_ptr<Texture> dyRes = std::dynamic_pointer_cast<Texture>(res);
    dyRes->m_textureFilter = m_textureFilter;
}

void Texture::CreateTextureSampler(VkFilter filter)
{
    Renderer* renderer = Application::GetApplication()->GetRenderer();

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = filter;
    samplerInfo.minFilter = filter;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    //need to add support for anisotropy, something needs to be initialized in the renderer for this
    samplerInfo.anisotropyEnable = VK_FALSE; //might be worth making conditional later, refer back to conclusion in https://vulkan-tutorial.com/Texture_mapping/Image_view_and_sampler

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(renderer->GetPhysicalDevice(), &properties);

    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE; //Coordinates are standard, between 0 - 1
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(m_iMipLevels);

    if (vkCreateSampler(renderer->GetLogicalDevice(), &samplerInfo, nullptr, &m_textureSampler) != VK_SUCCESS)
    {
        LogFatalError("Failed to create texture sampler.");
    }
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

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    static NobleSelectionList filterList;
    filterList.DoCombo("Filter Mode", m_bInitializeInterface, &m_textureFilter, { "Nearest", "Linear" });
    filterList.m_pResource = this;

    m_bInitializeInterface = false;
}

nlohmann::json Texture::AddToDatabase()
{
    nlohmann::json data;

    data["Filter"] = m_textureFilter;

    return data;
}

std::shared_ptr<Resource> Texture::LoadFromJson(const std::string& path, const nlohmann::json& data)
{
    std::shared_ptr<Texture> res = std::make_shared<Texture>();

    res->m_sLocalPath = path;
    res->m_sResourcePath = GetGameFolder() + path;
    res->m_textureFilter = data["Filter"];

    return res;
}

void Texture::SetDefaults(const nlohmann::json& data)
{
    m_textureFilter = data["Filter"];
}