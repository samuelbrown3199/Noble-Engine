#pragma once

#include "Resource.h"

struct Script : public Resource
{
	std::string m_sScriptString;

	Script();

	void OnLoad() override;
	void OnUnload() override;
};