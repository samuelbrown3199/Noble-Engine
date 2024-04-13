#include "SceneManagerWindow.h"

#include <Engine/Core/Application.h>
#include <Engine/Core/SceneManager.h>
#include <Engine/Core/ProjectFile.h>

#include "../EditorManagement/EditorManager.h"

void DeleteSceneModal::DoModal()
{
    SceneManagerWindow* sceneManagerWindow = dynamic_cast<SceneManagerWindow*>(m_pParentUI);
	EditorManager* editorManager = dynamic_cast<EditorManager*>(sceneManagerWindow->m_pEditor);
    SceneManager* sceneManager = Application::GetApplication()->GetSceneManager();
    ProjectFile* projectFile = Application::GetApplication()->GetProjectFile();

	CheckIfToggled();

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Delete Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
		ImGui::Text("Are you sure you want to delete this scene?\n\nThis action cannot be undone!");
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

        if (ImGui::Button("Delete"))
        {
            std::vector<std::string>* scenes = sceneManager->GetSceneList();

            std::string scenePath = GetGameFolder() + scenes->at(m_selectedSceneIndex);

            DeleteFilePath(scenePath);
            scenes->erase(scenes->begin() + m_selectedSceneIndex);
            projectFile->UpdateProjectFile();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void RenameSceneModal::DoModal()
{
    SceneManagerWindow* sceneManagerWindow = dynamic_cast<SceneManagerWindow*>(m_pParentUI);
	EditorManager* editorManager = dynamic_cast<EditorManager*>(sceneManagerWindow->m_pEditor);
	SceneManager* sceneManager = Application::GetApplication()->GetSceneManager();
	ProjectFile* projectFile = Application::GetApplication()->GetProjectFile();

	CheckIfToggled();

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Rename Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter the new name for the scene:");
        static char buf[64] = ""; ImGui::InputText("Scene Name", buf, 64);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

        if (ImGui::Button("Rename"))
        {
			std::vector<std::string>* scenes = sceneManager->GetSceneList();

            std::string scenePath = GetGameFolder() + scenes->at(m_selectedSceneIndex);
            std::string newScenePath = scenePath.erase(scenePath.find_last_of("\\"), scenePath.length()) + "\\" + buf + ".nsc";
            CutFile(scenePath, newScenePath);

            scenes->at(m_selectedSceneIndex) = GetFolderLocationRelativeToGameData(newScenePath);

			projectFile->UpdateProjectFile();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void SceneManagerWindow::InitializeInterface(ImGuiWindowFlags defaultFlags)
{
    EditorToolUI::InitializeInterface(defaultFlags);

    AddModal<DeleteSceneModal>("Delete Scene");
    AddModal<RenameSceneModal>("Rename Scene");
}

void SceneManagerWindow::UpdateOriginalSceneOrder()
{
	m_originalSceneOrder = *Application::GetApplication()->GetSceneManager()->GetSceneList();
}

void SceneManagerWindow::DoInterface()
{
	SceneManager* sceneManager = Application::GetApplication()->GetSceneManager();
    EditorManager* editorManager = dynamic_cast<EditorManager*>(m_pEditor);

	if (ImGui::Begin("Scene Manager", &m_uiOpen, m_windowFlags))
	{
        std::string item = "";
        bool selectedItem = false;

        std::vector<std::string>* scenes = sceneManager->GetSceneList();

        if (ImGui::TreeNode("Scenes"))
        {  
            selectedItem = m_selectedSceneIndex >= 0 && m_selectedSceneIndex < scenes->size();

            for (int n = 0; n < scenes->size(); n++)
            {
                item = scenes->at(n);
                bool thisSelected = m_selectedSceneIndex == n;
                if(ImGui::Selectable(item.c_str(), thisSelected))
                {
					m_selectedSceneIndex = n;
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
                    editorManager->LoadScene(n);
				}

                if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
                {
                    int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                    if (n_next >= 0 && n_next < scenes->size())
                    {
                        scenes->at(n) = scenes->at(n_next);
                        scenes->at(n_next) = item;
                        ImGui::ResetMouseDragDelta();

                        SetUnsavedWork(true);
                    }
                }
            }
            ImGui::TreePop();
        }
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Text("Double click to load scene. \nRe-order scenes by clicking and dragging. First scene in the list is the default scene of the project.");
        ImGui::Dummy(ImVec2(0, 10));

        if (ImGui::Button("Add Existing Scene to Project"))
        {
            IGFD::FileDialogConfig config;
            config.path = Application::GetApplication()->GetProjectFile()->m_sProjectDirectory;
            ImGuiFileDialog::Instance()->OpenDialog("ChooseSceneFile", "Choose Scene File", ".nsc", config);
		}
        ImGui::SetTooltip("Add an existing scene to the project.");

        if (ImGuiFileDialog::Instance()->Display("ChooseSceneFile"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                std::string scenePath = filePath + "\\" + fileName;
                scenePath = GetFolderLocationRelativeToGameData(scenePath);

                if (sceneManager->GetSceneIndex(scenePath) == -1)
                {
                    scenes->push_back(scenePath);
                    SetUnsavedWork(true);
                }
            }
            ImGuiFileDialog::Instance()->Close();
        }

        if (selectedItem)
        {
            ImGui::SameLine();
            if (ImGui::Button("Load Scene"))
            {
                editorManager->LoadScene(m_selectedSceneIndex);
            }
            ImGui::SetTooltip("Load the selected scene.");
            ImGui::SameLine();
            if (ImGui::Button("Remove Scene"))
            {
				scenes->erase(scenes->begin() + m_selectedSceneIndex);
                SetUnsavedWork(true);
			}
            ImGui::SetTooltip("Remove the selected scene from the project.");
            ImGui::SameLine();
            if (ImGui::Button("Delete Scene"))
            {
                dynamic_cast<DeleteSceneModal*>(GetModal("Delete Scene"))->m_selectedSceneIndex = m_selectedSceneIndex;
				DoModal("Delete Scene");
			}
            ImGui::SetTooltip("Delete the selected scene file and remove it from the project.");
			/*ImGui::SameLine();
            if (ImGui::Button("Rename Scene"))
            {
				dynamic_cast<RenameSceneModal*>(GetModal("Rename Scene"))->m_selectedSceneIndex = m_selectedSceneIndex;
                DoModal("Rename Scene");
            }
            ImGui::SetTooltip("Rename the selected scene file.");*/
        }

        if (ImGui::Button("Revert Scene Database"))
        {
            sceneManager->SetSceneList(m_originalSceneOrder);
            SetUnsavedWork(false);
        }
        ImGui::SetTooltip("Revert the scene order to the original order.");
        ImGui::SameLine();
        if (ImGui::Button("Save Scene Database"))
        {
            Application::GetApplication()->GetProjectFile()->UpdateProjectFile();
            UpdateOriginalSceneOrder();

            SetUnsavedWork(false);
        }
        ImGui::SetTooltip("Save the current scene order to the project file.");
	}
	ImGui::End();
}