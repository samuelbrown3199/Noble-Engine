#include "SceneHierarchyWindow.h"

#include <Engine/Core/Application.h>
#include <Engine/Core/ECS/Entity.h>
#include <Engine/Core/CommandTypes.h>

#include "../EditorManagement/EditorManager.h"

#include "DataEditorWindow.h"

void SceneHierarchyWindow::DoInterface()
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	EditorManager* editorManager = dynamic_cast<EditorManager*>(m_pEditor);

	ImGui::Begin("Scene Hierarchy", &m_uiOpen, m_windowFlags);

	UpdateWindowState();

	if(ImGui::Button("Undo TEST"))
		editorManager->GetCommandSystem()->Undo();
	ImGui::SameLine();
	if (ImGui::Button("Redo TEST"))
		editorManager->GetCommandSystem()->Redo();

	if (ImGui::Button("Create Entity"))
	{
		AddEntityCommand* command = new AddEntityCommand();
		editorManager->PushCommand(command);
	}
	if (m_iSelEntity != -1)
	{
		ImGui::SameLine();
		if (ImGui::Button("Delete Entity"))
		{
			DeleteEntityCommand* command = new DeleteEntityCommand(Application::GetApplication()->GetEntityList().at(m_iSelEntity).m_sEntityID);
			editorManager->PushCommand(command);
		}
	}

	std::vector<Entity>& entities = Application::GetApplication()->GetEntityList();
	if (ImGui::TreeNode("Entities"))
	{
		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		static int selection_mask = (1 << 2);
		for (int i = 0; i < entities.size(); i++)
			entities.at(i).DoEntityInterface(i, m_iSelEntity);

		ImGui::TreePop();
	}

	dynamic_cast<DataEditorWindow*>(editorManager->GetEditorUI("DataEditor"))->SetSelectedEntity(m_iSelEntity);

	ImGui::End();
}