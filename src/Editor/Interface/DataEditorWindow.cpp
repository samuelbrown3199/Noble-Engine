#include "DataEditorWindow.h"

#include <Engine/Core/Application.h>
#include <Engine/Core/ResourceManager.h>
#include <Engine/Core/ProjectFile.h>
#include <Engine/Core/ECS/Entity.h>

void DataEditorWindow::DoInterface()
{
	ResourceManager* resourceManager = Application::GetApplication()->GetResourceManager();

	ImGui::Begin("Data Editor", &m_uiOpen, m_windowFlags);

	UpdateWindowState();

	if (m_sSelEntity != "")
	{
		std::map<std::string, Entity>& entities = Application::GetApplication()->GetEntityList();
		NobleRegistry* registry = Application::GetApplication()->GetRegistry();
		std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = registry->GetComponentRegistry();

		Entity& entity = entities.at(m_sSelEntity);
		entity.DoEntityComponentInterface(compRegistry);
	}

	if (m_pSelResource != nullptr)
	{
		std::string type = m_pSelResource->m_resourceType + " Information";
		ImGui::SeparatorText(type.c_str());

		m_pSelResource->DoResourceInterface();

		if (ImGui::Button("Save Resource, To be removed.")) //this should be removed and the resource should automatically save when the user changes something
		{
			Application::GetApplication()->GetProjectFile()->UpdateProjectFile();
			m_pSelResource->ReloadResource();
		}
		/*ImGui::SameLine();
		if (ImGui::Button("Remove Resource"))
		{
			resourceManager->RemoveResourceFromDatabase(m_pSelResource->m_sLocalPath);
			Application::GetApplication()->GetProjectFile()->UpdateProjectFile();
			m_pSelResource = nullptr;
		}*/
	}

	ImGui::End();
}

void DataEditorWindow::SetSelectedEntity(std::string ID)
{ 
	if (m_sSelEntity == ID)
		return;

	m_sSelEntity = ID;
	m_pSelResource = nullptr;

	if(m_sSelEntity == "")
		return;

	//Initialize the entity's components interfaces
	Entity* ent = Application::GetApplication()->GetEntity(m_sSelEntity);
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
	m_sSelEntity = "";

	//Initialize the resource's interface
	m_pSelResource->InitializeInterface();
}