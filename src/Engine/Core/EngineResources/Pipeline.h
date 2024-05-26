#pragma once

#include "Resource.h"
#include "..\..\Core\Graphics\Renderer.h"

#include "..\..\Core\Registry.h"

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
	nlohmann::json AddToDatabase() override;
	std::shared_ptr<Resource> LoadFromJson(const std::string& path, const nlohmann::json& data) override;
	void SetDefaults(const nlohmann::json& data) override;
};

struct Pipeline : public Resource
{

private:

public:
	Pipeline();
	~Pipeline();

	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_pipeline;

	std::vector<std::pair<std::string, PushConstantRegistry*>> m_vPushConstants;
	std::vector<std::pair<std::string, DescriptorRegistry*>> m_vDescriptors;

	enum PipelineType
	{
		Graphics,
		Compute
	};
	int m_pipelineType;

	//Currently only concerned about these shader types, will need extending in the future.
	std::string m_vertexShaderPath = "none";
	std::string m_fragmentShaderPath = "none";

	void OnLoad() override;
	void OnUnload() override;

	void AddDescriptor();
	void AddDescriptor(std::string name);
	void ChangeDescriptor(int index, std::string name);

	void AddPushConstant();
	void AddPushConstant(std::string name);
	void ChangePushConstant(int index, std::string name);

	virtual void DoResourceInterface() override;

	virtual void AddResource(std::string path) override;
	nlohmann::json AddToDatabase() override;
	std::shared_ptr<Resource> LoadFromJson(const std::string& path, const nlohmann::json& data) override;

	void CreatePipeline();
};