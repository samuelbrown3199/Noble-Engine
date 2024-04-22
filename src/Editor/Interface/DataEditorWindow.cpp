#include "DataEditorWindow.h"

#include <Engine/Core/Application.h>
#include <Engine/Core/ECS/Entity.hpp>

void DataEditorWindow::DoInterface()
{
	ImGui::Begin("Data Editor", &m_uiOpen, m_windowFlags);

	UpdateWindowState();

	if (m_iSelEntity != -1)
	{
		std::vector<Entity>& entities = Application::GetApplication()->GetEntityList();
		NobleRegistry* registry = Application::GetApplication()->GetRegistry();
		std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = registry->GetComponentRegistry();
		std::vector<std::pair<std::string, Behaviour*>>* behaviourRegistry = registry->GetBehaviourRegistry();

		Entity& entity = entities.at(m_iSelEntity);
		entity.DoEntityComponentInterface(compRegistry, behaviourRegistry);
	}

	ImGui::End();
}	