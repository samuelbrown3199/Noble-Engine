#include "Texture.h"
#include "../../Useful.h"
#include "../../Core/Logger.h"
#include "../../Core/Application.h"
#include "../../Core/Graphics/Renderer.h"

#include "../../Core/ResourceManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Texture::OnLoad()
{
    if (m_bIsLoaded)
        return;

    stbi_uc* pixels = stbi_load(m_sResourcePath.c_str(), &m_iWidth, &m_iHeight, &m_iTexChannels, STBI_rgb_alpha);

    m_iMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_iWidth, m_iHeight)))) + 1;
    VkDeviceSize imageSize = m_iWidth * m_iHeight * 4;

    if (!pixels)
    {
        Logger::LogError("Failed to load texture image.", 2);
    }

    Renderer* renderer = Application::GetRenderer();

    m_texture = renderer->CreateImage(pixels, VkExtent3D(m_iWidth, m_iHeight, 1), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);
    CreateTextureSampler(m_textureFilter);
    m_bIsLoaded = true;
}

void Texture::OnUnload()
{
    if (!m_bIsLoaded)
        return;

    Renderer* renderer = Application::GetRenderer();

    vkDestroySampler(Renderer::GetLogicalDevice(), m_textureSampler, nullptr);
    vkDestroyImageView(renderer->GetLogicalDevice(), m_texture.m_imageView, nullptr);
    vmaDestroyImage(renderer->GetAllocator(), m_texture.m_image, m_texture.m_allocation);
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
    dyRes->m_textureFilter = m_textureFilter;
}


void Texture::GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
/*    VkCommandBuffer commandBuffer = Renderer::BeginSingleTimeCommand();

    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(Renderer::GetPhysicalDevice(), imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
        Logger::LogError("Texture image format does not support linear blitting.", 2);
    }

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    Renderer::EndSingleTimeCommands(commandBuffer);*/
}

void Texture::CreateTextureSampler(VkFilter filter)
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = filter;
    samplerInfo.minFilter = filter;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE; //might be worth making conditional later, refer back to conclusion in https://vulkan-tutorial.com/Texture_mapping/Image_view_and_sampler

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(Renderer::GetPhysicalDevice(), &properties);

    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE; //Coordinates are standard, between 0 - 1
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(m_iMipLevels);

    if (vkCreateSampler(Renderer::GetLogicalDevice(), &samplerInfo, nullptr, &m_textureSampler) != VK_SUCCESS)
    {
        Logger::LogError("Failed to create texture sampler.", 2);
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
    ImGui::Text(m_resourceType.c_str());

    if (m_bIsLoaded)
    {
        ImGui::Text("Texture is currently in use and can't be modified.");
        return;
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    const char* items[] = { "Nearest", "Linear" };
    int item = m_textureFilter;
    ImGui::Combo("Filter Mode", &item, items, IM_ARRAYSIZE(items));

    m_textureFilter = (VkFilter)item;
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