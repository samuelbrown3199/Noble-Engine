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

    Renderer* renderer = Application::GetApplication()->GetRenderer();

    //When we load the shader that hasnt been compiled, we need to load the code in for compilation. Otherwise we create a shader module like normal
    if(m_sCompiledShaderPath == "" || !PathExists(m_sCompiledShaderPath))
	{
        //open the shader file and read in the code
        std::ifstream file(m_sResourcePath, std::ios::ate | std::ios::binary);
        if (!file.is_open())
            LogError(FormatString("Failed to open shader file: %s", m_sLocalPath));

        size_t fileSize = (size_t)file.tellg();
        m_sShaderCode.resize(fileSize);

        file.seekg(0);
        file.read(m_sShaderCode.data(), fileSize);

        file.close();

        return;
	}

    if (!vkutil::LoadShaderModule(m_sCompiledShaderPath.c_str(), renderer->GetLogicalDevice(), &m_shaderModule))
        LogError(FormatString("Failed to load shader module: %s", m_sLocalPath));
}

void Shader::OnUnload()
{
    Renderer* renderer = Application::GetApplication()->GetRenderer();
    vkDestroyShaderModule(renderer->GetLogicalDevice(), m_shaderModule, nullptr);
}

void Shader::DoResourceInterface()
{
    static NobleSelectionList shaderTypeList;
    shaderTypeList.DoCombo("Shader Type", m_bInitializeInterface, &m_shaderType, { "Vertex", "Tesselation Control", "Tesselation Evaluation", "Geometry", "Fragment", "Compute" });
}

void Shader::AddResource(std::string path)
{
    ResourceManager* resourceManager = Application::GetApplication()->GetResourceManager();
    resourceManager->AddNewResource<Shader>(path);
}

nlohmann::json Shader::AddToDatabase()
{
    nlohmann::json data;

    data["ShaderType"] = m_shaderType;
    data["CompiledShaderPath"] = m_sCompiledShaderPath;
    data["CompiledShaderName"] = m_sCompiledShaderName;

    return data;
}

std::shared_ptr<Resource> Shader::LoadFromJson(const std::string& path, const nlohmann::json& data)
{
    std::shared_ptr<Shader> res = std::make_shared<Shader>();

    res->m_sLocalPath = path;
    res->m_sResourcePath = GetGameFolder() + path;
    res->m_shaderType = data["ShaderType"];

    if(data.find("CompiledShaderPath") != data.end())
		res->m_sCompiledShaderPath = data["CompiledShaderPath"];
    if(data.find("CompiledShaderName") != data.end())
        res->m_sCompiledShaderName = data["CompiledShaderName"];

    return res;
}

void Shader::SetDefaults(const nlohmann::json& data)
{
    m_shaderType = data["ShaderType"];
}

//----------------------------------------------------------------------------------------------------------------------------

Pipeline::Pipeline()
{
    m_resourceType = "Pipeline";
    m_bIsLoaded = false;
}

Pipeline::~Pipeline()
{
    if (m_bIsLoaded)
        OnUnload();
}

void Pipeline::OnLoad()
{
    if (!m_bIsLoaded)
        m_bIsLoaded = CreatePipeline();
}

void Pipeline::OnUnload()
{
    if (!m_bIsLoaded)
        return;

    Renderer* renderer = Application::GetApplication()->GetRenderer();

    vkDestroyPipelineLayout(renderer->GetLogicalDevice(), m_pipelineLayout, nullptr);
    vkDestroyPipeline(renderer->GetLogicalDevice(), m_pipeline, nullptr);

    m_bIsLoaded = false;
}

void Pipeline::AddDescriptor()
{
    m_vDescriptors.push_back(std::make_pair("None", nullptr));
}

void Pipeline::AddDescriptor(std::string name)
{
    m_vDescriptors.push_back(std::make_pair(name, Application::GetApplication()->GetRegistry()->GetDescriptorFromName(name)));
}

void Pipeline::ChangeDescriptor(int index, std::string name)
{
    m_vDescriptors.at(index).first = name;
    m_vDescriptors.at(index).second = Application::GetApplication()->GetRegistry()->GetDescriptorFromName(name);
}

void Pipeline::AddPushConstant()
{
    m_vPushConstants.push_back(std::make_pair("None", nullptr));
}

void Pipeline::AddPushConstant(std::string name)
{
    m_vPushConstants.push_back(std::make_pair(name, Application::GetApplication()->GetRegistry()->GetPushConstantFromName(name)));
}

void Pipeline::ChangePushConstant(int index, std::string name)
{
    m_vPushConstants.at(index).first = name;
    m_vPushConstants.at(index).second = Application::GetApplication()->GetRegistry()->GetPushConstantFromName(name);
}

void Pipeline::DoResourceInterface()
{
    static NobleSelectionList pipelineTypeList;
    pipelineTypeList.DoCombo("Pipeline Type", m_bInitializeInterface, &m_pipelineType, { "Graphics", "Compute" });

    if (m_pipelineType == Graphics)
    {
        static ResourceSelectionWidget vertexShaderWidget;
        vertexShaderWidget.m_pResource = this;
        vertexShaderWidget.m_sResourceType = "Shader";
        vertexShaderWidget.DoResourceSelection("Vertex Shader", m_bInitializeInterface, &m_vertexShaderPath);
        
        static ResourceSelectionWidget fragmentShaderWidget;
        fragmentShaderWidget.m_pResource = this;
        fragmentShaderWidget.m_sResourceType = "Shader";
        fragmentShaderWidget.DoResourceSelection("Fragment Shader", m_bInitializeInterface, &m_fragmentShaderPath);
    }

    ImGui::Text("Pipeline Push Constants");
    std::vector<std::pair<std::string, PushConstantRegistry>>* constantRegistry = Application::GetApplication()->GetRegistry()->GetPushConstantRegistry();
    for (int i = 0; i < m_vPushConstants.size(); i++)
    {
        if (ImGui::BeginMenu(m_vPushConstants.at(i).first.c_str()))
        {
            for (int o = 0; o < constantRegistry->size(); o++)
            {
                if (ImGui::Button(constantRegistry->at(o).first.c_str()))
                    ChangePushConstant(i, constantRegistry->at(o).first);
            }

            ImGui::EndMenu();
        }
    }
    if (ImGui::Button("Add Push Constant"))
        AddPushConstant();

    ImGui::Text("Pipeline Descriptors");
    std::vector<std::pair<std::string, DescriptorRegistry>>* descriptorRegistry = Application::GetApplication()->GetRegistry()->GetDescriptorRegistry();

    for (int i = 0; i < m_vDescriptors.size(); i++)
    {
        if (ImGui::BeginMenu(m_vDescriptors.at(i).first.c_str()))
        {
            for (int o = 0; o < descriptorRegistry->size(); o++)
            {
                if (ImGui::Button(descriptorRegistry->at(o).first.c_str()))
                    ChangeDescriptor(i, descriptorRegistry->at(o).first);
            }

            ImGui::EndMenu();
        }
    }
    if (ImGui::Button("Add Descriptor"))
        AddDescriptor();

    if(ImGui::Button("Recreate Pipeline"))
    {
        CreatePipeline();
    }
}

nlohmann::json Pipeline::AddToDatabase()
{
    nlohmann::json data;

    data["PipelineType"] = m_pipelineType;
    if (!m_vertexShaderPath.empty())
        data["VertexShader"] = m_vertexShaderPath;
    if (!m_fragmentShaderPath.empty())
        data["FragmentShader"] = m_fragmentShaderPath;
    if (m_vDescriptors.size() != 0)
    {
        std::vector<std::string> descriptors;
        for (int i = 0; i < m_vDescriptors.size(); i++)
        {
            descriptors.push_back(m_vDescriptors.at(i).first);
        }

        data["Descriptors"] = descriptors;
    }

    if (m_vPushConstants.size() != 0)
    {
        std::vector<std::string> pushconstants;
        for (int i = 0; i < m_vPushConstants.size(); i++)
        {
            pushconstants.push_back(m_vPushConstants.at(i).first);
        }

        data["PushConstants"] = pushconstants;
    }

    return data;
}

void Pipeline::AddResource(std::string path)
{
    ResourceManager* resourceManager = Application::GetApplication()->GetResourceManager();
    resourceManager->AddNewResource<Pipeline>(path);
}

std::shared_ptr<Resource> Pipeline::LoadFromJson(const std::string& path, const nlohmann::json& data)
{
    std::shared_ptr<Pipeline> res = std::make_shared<Pipeline>();

    res->m_sLocalPath = path;
    res->m_sResourcePath = path;

    if (data.find("PipelineType") != data.end())
        res->m_pipelineType = data["PipelineType"];
    if (data.find("VertexShader") != data.end())
        res->m_vertexShaderPath = data["VertexShader"];
    if (data.find("FragmentShader") != data.end())
        res->m_fragmentShaderPath = data["FragmentShader"];
    if (data.find("Descriptors") != data.end())
    {
        for (int i = 0; i < data["Descriptors"].size(); i++)
        {
            res->AddDescriptor(data["Descriptors"][i]);
        }
    }
    if (data.find("PushConstants") != data.end())
    {
        for (int i = 0; i < data["PushConstants"].size(); i++)
        {
            res->AddPushConstant(data["PushConstants"][i]);
        }
    }

    return res;
}

bool Pipeline::CreatePipeline()
{
    Renderer* renderer = Application::GetApplication()->GetRenderer();
    ResourceManager* rManager = Application::GetApplication()->GetResourceManager();

    if (m_pipelineType == PipelineType::Graphics)
    {
        std::shared_ptr<Shader> vertexShader = nullptr;
        std::shared_ptr<Shader> fragmentShader = nullptr;

        if (m_vertexShaderPath.empty())
        {
            LogError(FormatString("Pipeline %s does not have a vertex shader set.", m_sResourcePath));
            return false;
        }
        if (m_fragmentShaderPath.empty())
        {
            LogError(FormatString("Pipeline %s does not have a fragment shader set.", m_sResourcePath));
            return false;
        }

        vertexShader = rManager->LoadResource<Shader>(m_vertexShaderPath);
        fragmentShader = rManager->LoadResource<Shader>(m_fragmentShaderPath);

        if (vertexShader->m_sCompiledShaderPath.empty() || fragmentShader->m_sCompiledShaderPath.empty())
        {
            LogError("A shader has not been compiled, cannot create pipeline.");
            return false;
        }

        std::vector<VkPushConstantRange> pushConstants;
        for (int i = 0; i < m_vPushConstants.size(); i++)
        {
            if (m_vPushConstants.at(i).second != nullptr)
            {
                pushConstants.push_back(m_vPushConstants.at(i).second->m_pushConstantRange);

                if (pushConstants.size() != 1)
                {
                    pushConstants.at(i).offset = pushConstants.at(i - 1).size;
                }
            }
        }

        std::vector<VkDescriptorSetLayout> descriptorLayouts;
        for (int i = 0; i < m_vDescriptors.size(); i++)
        {
            if(m_vDescriptors.at(i).second != nullptr)
                descriptorLayouts.push_back(*m_vDescriptors.at(i).second->m_layout);
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = vkinit::PipelineLayoutCreateInfo();
        pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();
        pipelineLayoutInfo.pushConstantRangeCount = pushConstants.size();
        pipelineLayoutInfo.pSetLayouts = descriptorLayouts.data();
        pipelineLayoutInfo.setLayoutCount = descriptorLayouts.size();

        if (vkCreatePipelineLayout(renderer->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
            LogFatalError(FormatString("Failed to create pipeline layout for pipeline %s", m_sResourcePath.c_str()));

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

        //Some/all of these settings should be parameters to be changed in the pipeline resource. ^^

        //finally build the pipeline
        m_pipeline = pipelineBuilder.BuildPipeline(renderer->GetLogicalDevice());

        return true;
    }

    LogInfo("Created pipeline " + m_sLocalPath);
}