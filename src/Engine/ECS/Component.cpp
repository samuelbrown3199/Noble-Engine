#include "Component.h"

#include "../Core/Registry.h"

Component* Component::GetAsComponent(std::string entityID)
{
	Datalist* dataList = NobleRegistry::GetComponentList(GetComponentID());
	return dataList->GetComponent(entityID);
}

void Component::AddComponent()
{
	Datalist* dataList = NobleRegistry::GetComponentList(GetComponentID());
	dataList->AddComponent(this);
}

void Component::AddComponentToEntity(std::string entityID)
{
	Datalist* dataList = NobleRegistry::GetComponentList(GetComponentID());
	dataList->AddComponentToEntity(entityID);
}

void Component::RemoveComponent(std::string entityID)
{
	Datalist* dataList = NobleRegistry::GetComponentList(GetComponentID());
	dataList->RemoveComponent(entityID);
}

void Component::RemoveAllComponents()
{
	Datalist* dataList = NobleRegistry::GetComponentList(GetComponentID());
	dataList->RemoveAllComponents();
}

void Component::Update(bool useThreads, int maxComponentsPerThread)
{
	Datalist* dataList = NobleRegistry::GetComponentList(GetComponentID());
	dataList->Update(useThreads, maxComponentsPerThread);
}

void Component::Render(bool useThreads, int maxComponentsPerThread)
{
	Datalist* dataList = NobleRegistry::GetComponentList(GetComponentID());
	dataList->Render(useThreads, maxComponentsPerThread);
}

void Component::LoadComponentDataFromJson(nlohmann::json& j)
{
	Datalist* dataList = NobleRegistry::GetComponentList(GetComponentID());
	dataList->LoadComponentDataFromJson(j);
}

nlohmann::json Component::WriteComponentDataToJson()
{
	Datalist* dataList = NobleRegistry::GetComponentList(GetComponentID());
	return dataList->WriteComponentDataToJson();
}