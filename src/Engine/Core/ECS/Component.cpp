#include "Component.h"

#include "../Registry.h"

int Component::GetComponentIndex(std::string entityID)
{
	Datalist* dataList = NobleRegistry::GetComponentList(GetComponentID());
	return dataList->GetComponentIndex(entityID);
}

Component* Component::GetAsComponent(int index)
{
	Datalist* dataList = NobleRegistry::GetComponentList(GetComponentID());
	return dataList->GetComponent(index);
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

void Component::PreRender(bool useThreads, int maxComponentsPerThread)
{
	Datalist* dataList = NobleRegistry::GetComponentList(GetComponentID());
	dataList->PreRender(useThreads, maxComponentsPerThread);
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