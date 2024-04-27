#include "SceneHierarchyWindow.h"

#include <Engine/Core/Application.h>
#include <Engine/Core/InputManager.h>
#include <Engine/Core/ECS/Entity.h>
#include <Engine/Core/CommandTypes.h>

#include "../EditorManagement/EditorManager.h"

#include "DataEditorWindow.h"

void CreateEntityModal::CreateEntity()
{
	SceneHierarchyWindow* sceneHierarchy = dynamic_cast<SceneHierarchyWindow*>(m_pParentUI);
	EditorManager* editorManager = dynamic_cast<EditorManager*>(sceneHierarchy->m_pEditor);

	AddEntityCommand* command = new AddEntityCommand(m_sEntityName, "");
	editorManager->PushCommand(command);

	m_sEntityName = "New Entity";

	ImGui::CloseCurrentPopup();
}

void CreateEntityModal::DoModal()
{
	SceneHierarchyWindow* sceneHierarchy = dynamic_cast<SceneHierarchyWindow*>(m_pParentUI);
	EditorManager* editorManager = dynamic_cast<EditorManager*>(sceneHierarchy->m_pEditor);
	InputManager* iManager = Application::GetApplication()->GetInputManager();

	CheckIfToggled();

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal(m_sID.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::InputText("Entity Name", &m_sEntityName, 128);

		if (iManager->GetKeyDown(SDLK_KP_ENTER) || iManager->GetKeyDown(SDLK_RETURN))
			CreateEntity();

		if (ImGui::Button("Create"))
		{
			CreateEntity();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}

void SceneHierarchyWindow::InitializeInterface(ImGuiWindowFlags defaultFlags)
{
	EditorToolUI::InitializeInterface(defaultFlags);

	AddModal<CreateEntityModal>("Create Entity");
}

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
		InputManager* iManager = Application::GetApplication()->GetInputManager();

		if (iManager->GetKey(SDLK_LSHIFT) || iManager->GetKey(SDLK_RSHIFT))
		{
			AddEntityCommand* command = new AddEntityCommand("New Entity", "");
			editorManager->PushCommand(command);
		}
		else
			DoModal("Create Entity");
	}
	//ImGui::SetTooltip("Allows the creation of a new entity, with a name of your choosing. If shift is pressed it will use a default name.");

	if (m_iSelEntity != -1)
	{
		ImGui::SameLine();
		if (ImGui::Button("Delete Entity"))
		{
			DeleteEntityCommand* command = new DeleteEntityCommand(Application::GetApplication()->GetEntityList().at(m_iSelEntity).m_sEntityID);
			editorManager->PushCommand(command);
		}
		//ImGui::SetTooltip("Deletes the selected entity.");
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