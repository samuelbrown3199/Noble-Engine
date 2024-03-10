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
	Pipeline();
	~Pipeline();

	enum PipelineType
	{
		Graphics,
		Compute
	};
	PipelineType m_pipelineType;

	void OnLoad() override;
	void OnUnload() override;

	virtual void DoResourceInterface() override;

	virtual void AddResource(std::string path) override;
	virtual std::vector<std::shared_ptr<Resource>> GetResourcesOfType() override;
	nlohmann::json AddToDatabase() override;
	std::shared_ptr<Resource> LoadFromJson(const std::string& path, const nlohmann::json& data) override;
};