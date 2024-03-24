#pragma once

#include "Resource.h"
#include "..\..\Core\Graphics\Renderer.h"

struct Shader : public Resource
{
	Shader();
	~Shader();

	void OnLoad() override;
	void OnUnload() override;

	enum ShaderType
	{
		vertex,
		tesselationCont,
		tesselationEval,
		geometry,
		fragment,
		compute
	};
	ShaderType m_shaderType;
	VkShaderModule m_shaderModule;

	virtual void DoResourceInterface() override;

	virtual void AddResource(std::string path) override;
	virtual std::vector<std::shared_ptr<Resource>> GetResourcesOfType() override;
	nlohmann::json AddToDatabase() override;
	std::shared_ptr<Resource> LoadFromJson(const std::string& path, const nlohmann::json& data) override;
	void SetDefaults(const nlohmann::json& data) override;
};

struct Pipeline : public Resource
{

private:

	std::string ChangeShader(Shader::ShaderType type, std::string currentPath, std::string elementName);

public:
	Pipeline();
	~Pipeline();

	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_pipeline;

	enum PipelineType
	{
		Graphics,
		Compute
	};
	PipelineType m_pipelineType;

	//Currently only concerned about these shader types, will need extending in the future.
	std::string m_vertexShaderPath;
	std::string m_fragmentShaderPath;

	void OnLoad() override;
	void OnUnload() override;

	virtual void DoResourceInterface() override;

	virtual void AddResource(std::string path) override;
	virtual std::vector<std::shared_ptr<Resource>> GetResourcesOfType() override;
	nlohmann::json AddToDatabase() override;
	std::shared_ptr<Resource> LoadFromJson(const std::string& path, const nlohmann::json& data) override;

	void CreatePipeline();
};