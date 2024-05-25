#include "Component.h"

#include "../ResourceManager.h"
#include "../Registry.h"

int Component::GetComponentIndex(std::string entityID)
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Datalist* dataList = registry->GetComponentList(GetComponentID());
	return dataList->GetComponentIndex(entityID);
}

Component* Component::GetAsComponent(int index)
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Datalist* dataList = registry->GetComponentList(GetComponentID());
	return dataList->GetComponent(index);
}

void Component::AddComponent()
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Datalist* dataList = registry->GetComponentList(GetComponentID());
	dataList->AddComponent(this);
}

void Component::AddComponentToEntity(std::string entityID)
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Datalist* dataList = registry->GetComponentList(GetComponentID());
	dataList->AddComponentToEntity(entityID);
}

void Component::RemoveComponent(std::string entityID)
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Datalist* dataList = registry->GetComponentList(GetComponentID());
	dataList->RemoveComponent(entityID);
}

void Component::RemoveAllComponents()
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Datalist* dataList = registry->GetComponentList(GetComponentID());
	dataList->RemoveAllComponents();
}

void Component::Update(bool useThreads, int maxComponentsPerThread)
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Datalist* dataList = registry->GetComponentList(GetComponentID());
	dataList->Update(useThreads, maxComponentsPerThread);
}

void Component::PreRender(bool useThreads, int maxComponentsPerThread)
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Datalist* dataList = registry->GetComponentList(GetComponentID());
	dataList->PreRender(useThreads, maxComponentsPerThread);
}

void Component::LoadComponentDataFromJson(nlohmann::json& j)
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Datalist* dataList = registry->GetComponentList(GetComponentID());
	dataList->LoadComponentDataFromJson(j);
}

nlohmann::json Component::WriteComponentDataToJson()
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Datalist* dataList = registry->GetComponentList(GetComponentID());
	return dataList->WriteComponentDataToJson();
}