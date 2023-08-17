#include "ScriptEmbedder.h"
#include "../Core/Application.h"
#include "../ECS/Entity.hpp"

ComponentDatalist<ScriptEmbedder> ScriptEmbedder::m_componentList;

void ScriptEmbedder::AddComponent()
{
	m_componentList.AddComponent(this);
}

void ScriptEmbedder::AddComponentToEntity(std::string entityID)
{
	m_componentList.AddComponentToEntity(entityID);
	Application::GetEntity(entityID)->GetAllComponents();
}

void ScriptEmbedder::RemoveComponent(std::string entityID)
{
	m_componentList.RemoveComponent(entityID);
}

void ScriptEmbedder::RemoveAllComponents()
{
	m_componentList.RemoveAllComponents();
}

ScriptEmbedder* ScriptEmbedder::GetComponent(std::string entityID)
{
	return m_componentList.GetComponent(entityID);
}

void ScriptEmbedder::Update(bool useThreads, int maxComponentsPerThread) 
{
	m_componentList.Update(useThreads, maxComponentsPerThread);
}

void ScriptEmbedder::OnUpdate()
{
	if (m_script == nullptr)
		return;

	m_luaState->RunScript(m_script);
	m_luaState->RunScriptFunction(m_sFunctionName);
}

void ScriptEmbedder::Render(bool useThreads, int maxComponentsPerThread){}

void ScriptEmbedder::LoadComponentDataFromJson(nlohmann::json& j)
{
	m_componentList.LoadComponentDataFromJson(j);
}

nlohmann::json ScriptEmbedder::WriteComponentDataToJson()
{
	return m_componentList.WriteComponentDataToJson();
}