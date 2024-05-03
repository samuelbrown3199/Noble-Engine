#include "DataEditorWindow.h"

#include <Engine/Core/Application.h>
#include <Engine/Core/ECS/Entity.h>

void DataEditorWindow::DoInterface()
{
	ImGui::Begin("Data Editor", &m_uiOpen, m_windowFlags);

	UpdateWindowState();

	if (m_iSelEntity != -1)
	{
		std::vector<Entity>& entities = Application::GetApplication()->GetEntityList();
		NobleRegistry* registry = Application::GetApplication()->GetRegistry();
		std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = registry->GetComponentRegistry();

		Entity& entity = entities.at(m_iSelEntity);
		entity.DoEntityComponentInterface(compRegistry);
	}

	if (m_pSelResource != nullptr)
	{
		m_pSelResource->DoResourceInterface();
	}

	ImGui::End();
}

void DataEditorWindow::SetSelectedEntity(int _index)
{ 
	if (_index < 0)
		return;

	if (_index >= Application::GetApplication()->GetEntityList().size())
		return;

	if (m_iSelEntity == _index)
		return;

	m_iSelEntity = _index;
	m_pSelResource = nullptr;

	//Initialize the entity's components interfaces
	Entity* ent = Application::GetApplication()->GetEntity(m_iSelEntity);
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();

	std::map<std::string, int>::iterator itr;
	for (itr = ent->m_vComponents.begin(); itr != ent->m_vComponents.end(); itr++)
	{
		registry->GetComponentList(itr->first)->GetComponent(itr->second)->InitializeComponentInterface();
	}
}

void DataEditorWindow::SetSelectedResource(std::shared_ptr<Resource> _resource)
{
	if (_resource == nullptr)
		return;

	if (m_pSelResource == _resource)
		return;

	m_pSelResource = _resource;
	m_iSelEntity = -1;

	//Initialize the resource's interface
	m_pSelResource->InitializeInterface();
}