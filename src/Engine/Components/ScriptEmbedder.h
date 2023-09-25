#pragma once

#include "../ECS/Component.h"
#include "../ECS/ComponentList.hpp"

#include "../Core/ResourceManager.h"
#include "../Lua/NobleLuaState.h"
#include "../Resource/Script.h"

struct ScriptEmbedder : public Component
{
	std::shared_ptr <NobleLuaState> m_luaState;
	std::shared_ptr<Script> m_script;

	std::string m_sFunctionName;

	std::string GetComponentID() override
	{
		return "ScriptEmbedder";
	}

	void OnInitialize() override
	{
		m_luaState = std::make_shared<NobleLuaState>();
	}

	nlohmann::json WriteJson()
	{
		nlohmann::json data;

		data = { {"functionName", m_sFunctionName }, {"scriptPath", m_script->m_sLocalPath} };

		return data;
	}

	void FromJson(const nlohmann::json& j)
	{
		m_sFunctionName = j["functionName"];
		m_script = ResourceManager::LoadResource<Script>(j["scriptPath"]);
	}

	virtual void DoComponentInterface() override
	{
		m_script = ResourceManager::DoResourceSelectInterface<Script>("Script", m_script != nullptr ? m_script->m_sLocalPath : "none");
		ImGui::InputText("Entry Function", &m_sFunctionName);
	}

	virtual void OnUpdate() override;
};