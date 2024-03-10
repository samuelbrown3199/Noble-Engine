#include "Pipeline.h"

#include "../../Core/Graphics/VulkanTypes.h"
#include "../../Core/ResourceManager.h"

#include "../../Core/Graphics/VulkanPipelines.h"

Shader::Shader()
{
    m_resourceType = "Shader";
}

Shader::~Shader()
{
    if (m_bIsLoaded)
        OnUnload();
}

void Shader::OnLoad()
{
    Resource::OnLoad();

    if (!vkutil::LoadShaderModule(m_sResourcePath.c_str(), Renderer::GetLogicalDevice(), &m_shaderModule))
        Logger::LogError(FormatString("Failed to load shader module: %s", m_sLocalPath), 1);
}

void Shader::OnUnload()
{
    vkDestroyShaderModule(Renderer::GetLogicalDevice(), m_shaderModule, nullptr);
}

void Shader::DoResourceInterface()
{
    int val = m_shaderType;
    ImGui::DragInt("Shader Type", &val, 1, 0, 5);

    m_shaderType = (ShaderType)val;
}

void Shader::AddResource(std::string path)
{
    ResourceManager::AddNewResource<Shader>(path);
}

std::vector<std::shared_ptr<Resource>> Shader::GetResourcesOfType()
{
    return ResourceManager::GetAllResourcesOfType<Shader>();
}

nlohmann::json Shader::AddToDatabase()
{
    nlohmann::json data;

    data["ShaderType"] = m_shaderType;

    return data;
}

std::shared_ptr<Resource> Shader::LoadFromJson(const std::string& path, const nlohmann::json& data)
{
    std::shared_ptr<Shader> res = std::make_shared<Shader>();

    res->m_sLocalPath = path;
    res->m_sResourcePath = GetGameFolder() + path;
    res->m_shaderType = data["ShaderType"];

    return res;
}

void Shader::SetDefaults(const nlohmann::json& data)
{
    m_shaderType = data["ShaderType"];
}




Pipeline::Pipeline()
{
    m_resourceType = "Pipeline";
}

Pipeline::~Pipeline()
{
    if (m_bIsLoaded)
        OnUnload();
}

void Pipeline::OnLoad()
{

}

void Pipeline::OnUnload()
{
    if (!m_bIsLoaded)
        return;

    m_bIsLoaded = false;
}

void Pipeline::DoResourceInterface()
{
    int val = m_pipelineType;
    ImGui::DragInt("Pipeline Type", &val, 1, 0, 5); //should be a dropdown at some point.
    m_pipelineType = (PipelineType)val;
}

nlohmann::json Pipeline::AddToDatabase()
{
    nlohmann::json data;

    data["PipelineType"] = m_pipelineType;

    return data;
}

void Pipeline::AddResource(std::string path)
{
    ResourceManager::AddNewResource<Pipeline>(path);
}

std::vector<std::shared_ptr<Resource>> Pipeline::GetResourcesOfType()
{
    return ResourceManager::GetAllResourcesOfType<Pipeline>();
}

std::shared_ptr<Resource> Pipeline::LoadFromJson(const std::string& path, const nlohmann::json& data)
{
    std::shared_ptr<Pipeline> res = std::make_shared<Pipeline>();

    res->m_sLocalPath = path;
    res->m_sResourcePath = GetGameFolder() + path;

    return res;
}