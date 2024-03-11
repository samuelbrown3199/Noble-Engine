#include "Pipeline.h"

#include "../../Core/Application.h"

#include "../../Core/Graphics/VulkanTypes.h"
#include "../../Core/ResourceManager.h"
#include "../../Core/Graphics/VulkanInitialisers.h"
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
    Renderer* renderer = Application::GetRenderer();

    if (m_pipelineType == PipelineType::Graphics)
    {
        std::shared_ptr<Shader> vertexShader = nullptr;
        std::shared_ptr<Shader> fragmentShader = nullptr;

        if (!m_vertexShaderPath.empty())
        {
            vertexShader = ResourceManager::LoadResource<Shader>(m_vertexShaderPath);
            Logger::LogError(FormatString("Pipeline %s does not have a vertex shader set.", m_sResourcePath), 0);
            return;
        }
        if (!m_fragmentShaderPath.empty())
        {
            fragmentShader = ResourceManager::LoadResource<Shader>(m_fragmentShaderPath);
            Logger::LogError(FormatString("Pipeline %s does not have a fragment shader set.", m_sResourcePath), 0);
            return;
        }

        //Push constants and descriptor sets need to somehow be flexible.
        VkPushConstantRange bufferRange{};
        bufferRange.offset = 0;
        bufferRange.size = sizeof(GPUDrawPushConstants);
        bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = vkinit::PipelineLayoutCreateInfo();
        pipelineLayoutInfo.pPushConstantRanges = &bufferRange;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pSetLayouts = &renderer->m_singleImageDescriptorLayout; //very temp
        pipelineLayoutInfo.setLayoutCount = 1;

        if (vkCreatePipelineLayout(renderer->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
            Logger::LogError(FormatString("Failed to create pipeline layout for pipeline %s", m_sResourcePath.c_str()), 2);

        PipelineBuilder pipelineBuilder;
        //use the triangle layout we created
        pipelineBuilder.m_pipelineLayout = m_pipelineLayout;
        //connecting the vertex and pixel shaders to the pipeline
        pipelineBuilder.SetShaders(vertexShader->m_shaderModule, fragmentShader->m_shaderModule);
        //it will draw triangles
        pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        //filled triangles
        pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
        //no backface culling
        pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
        //no multisampling
        pipelineBuilder.SetMultisamplingNone();
        pipelineBuilder.DisableBlending();
        pipelineBuilder.EnableDepthTest(true, VK_COMPARE_OP_LESS_OR_EQUAL);

        //connect the image format we will draw into, from draw image
        pipelineBuilder.SetColorAttachmentFormat(renderer->GetDrawImageFormat());
        pipelineBuilder.SetDepthFormat(renderer->GetDepthImageFormat());

        //finally build the pipeline
        m_pipeline = pipelineBuilder.BuildPipeline(Renderer::GetLogicalDevice());
    }
}

void Pipeline::OnUnload()
{
    if (!m_bIsLoaded)
        return;

    vkDestroyPipelineLayout(Renderer::GetLogicalDevice(), m_pipelineLayout, nullptr);
    vkDestroyPipeline(Renderer::GetLogicalDevice(), m_pipeline, nullptr);

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
    if (!m_vertexShaderPath.empty())
        data["VertexShader"] = m_vertexShaderPath;
    if (!m_fragmentShaderPath.empty())
        data["FragmentShader"] = m_fragmentShaderPath;

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

    if (data.find("VertexShader") != data.end())
        m_vertexShaderPath = data["VertexShader"];

    if (data.find("FragmentShader") != data.end())
        m_vertexShaderPath = data["FragmentShader"];

    return res;
}