#pragma once

#include "../ECS/Component.hpp"
#include "../ECS/ComponentList.hpp"

#include "../Core/ResourceManager.h"
#include "../Lua/NobleLuaState.h"
#include "../Resource/Script.h"

struct ScriptEmbedder : public Component
{
	static ComponentDatalist<ScriptEmbedder> m_componentList;

	std::shared_ptr <NobleLuaState> m_luaState;
	std::shared_ptr<Script> m_script;

	std::string m_sFunctionName;

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

	Component* GetAsComponent(std::string entityID) override
	{
		return GetComponent(entityID);
	}

	virtual void DoComponentInterface() override
	{
		m_script = ResourceManager::DoResourceSelectInterface<Script>("Script", m_script != nullptr ? m_script->m_sLocalPath : "none");
		ImGui::InputText("Entry Function", &m_sFunctionName);
	}

	virtual void AddComponent() override;
	virtual void AddComponentToEntity(std::string entityID) override;
	virtual void RemoveComponent(std::string entityID) override;
	virtual void RemoveAllComponents() override;

	ScriptEmbedder* GetComponent(std::string entityID);

	virtual void Update(bool useThreads, int maxComponentsPerThread) override;
	virtual void OnUpdate() override;

	virtual void Render(bool useThreads, int maxComponentsPerThread) override;

	virtual void LoadComponentDataFromJson(nlohmann::json& j) override;
	virtual nlohmann::json WriteComponentDataToJson() override;
};