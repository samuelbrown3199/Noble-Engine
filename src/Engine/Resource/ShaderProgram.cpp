#include "ShaderProgram.h"

#include "../Core/ResourceManager.h"

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

    //nothing rn

    return data;
}

std::shared_ptr<Resource> Shader::LoadFromJson(const std::string& path, const nlohmann::json& data)
{
    std::shared_ptr<Shader> res = std::make_shared<Shader>();

    res->m_sLocalPath = path;
    res->m_sResourcePath = GetGameFolder() + path;

    return res;
}

void Shader::SetDefaults(const nlohmann::json& data)
{
    //nothing rn
}


void ShaderProgram::ChangeShader(std::shared_ptr<Shader> shader, std::shared_ptr<Shader>& targetShader)
{
	if (shader == nullptr)
		return;

	if (targetShader != nullptr && shader->m_sLocalPath == targetShader->m_sLocalPath)
		return;

	targetShader = shader;
}


void ShaderProgram::DoShaderProgramInterface()
{
	ChangeShader(ResourceManager::DoResourceSelectInterface<Shader>("Vertex Shader", m_vertexShader != nullptr ? m_vertexShader->m_sLocalPath : ""), m_vertexShader);
	ChangeShader(ResourceManager::DoResourceSelectInterface<Shader>("Fragment Shader", m_fragmentShader != nullptr ? m_fragmentShader->m_sLocalPath : ""), m_fragmentShader);

	if (ImGui::Button("Compile Program"))
	{
		BindShader(m_vertexShader, GL_VERTEX_SHADER);
		BindShader(m_fragmentShader, GL_FRAGMENT_SHADER);
	}

	ImGui::SameLine();

	if (ImGui::Button("Save Program"))
	{
		ResourceManager::WriteResourceDatabase();
	}
}

nlohmann::json ShaderProgram::WriteToJson()
{
	nlohmann::json data;

	if (m_vertexShader)
		data["Vertex"] = m_vertexShader->m_sLocalPath;
	if(m_fragmentShader)
		data["Fragment"] = m_fragmentShader->m_sLocalPath;

	return data;
}

void ShaderProgram::LoadFromJson(nlohmann::json j)
{
	if (j.find("Vertex") != j.end())
	{
		m_vertexShader = ResourceManager::LoadResource<Shader>(j.at("Vertex"));
		BindShader(m_vertexShader, GL_VERTEX_SHADER);
	}

	if (j.find("Fragment") != j.end())
	{
		m_fragmentShader = ResourceManager::LoadResource<Shader>(j.at("Fragment"));
		BindShader(m_fragmentShader, GL_FRAGMENT_SHADER);
	}
}