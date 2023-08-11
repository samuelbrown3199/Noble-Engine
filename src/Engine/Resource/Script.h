#pragma once

#include "Resource.h"

struct Script : public Resource
{
	std::string m_sScriptString;

	Script();

	void OnLoad() override;
	void OnUnload() override;

	virtual void AddResource(std::string path) override;
	virtual std::vector<std::shared_ptr<Resource>> GetResourcesOfType() override;
};