#pragma once

#include "Resource.h"

struct Script : public Resource
{
	std::string m_sScriptString;

	Script();

	void OnLoad() override;
	void OnUnload() override;

	virtual void AddResource(std::string path) override;

	std::shared_ptr<Resource> LoadFromJson(const std::string& path, const nlohmann::json& data) override;
};