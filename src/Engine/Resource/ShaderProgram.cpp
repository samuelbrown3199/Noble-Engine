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



void ShaderProgram::DoShaderProgramInterface()
{
	ImGui::Text("YOOOO SHADER PROGRAM EDITOR?!");

	ChangeShader(ResourceManager::DoResourceSelectInterface<Shader>("Vertex Shader", ""), m_vertexShader);
	ChangeShader(ResourceManager::DoResourceSelectInterface<Shader>("Fragment Shader", ""), m_fragmentShader);

	if (ImGui::Button("Compile Program"))
	{
		BindShader(m_vertexShader, GL_VERTEX_SHADER);
		BindShader(m_fragmentShader, GL_FRAGMENT_SHADER);
	}
}