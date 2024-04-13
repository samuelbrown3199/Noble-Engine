#include "SceneManagerWindow.h"

#include <Engine/Core/Application.h>
#include <Engine/Core/SceneManager.h>
#include <Engine/Core/ProjectFile.h>

void SceneManagerWindow::InitializeInterface()
{
}

void SceneManagerWindow::UpdateOriginalSceneOrder()
{
	m_originalSceneOrder = *Application::GetApplication()->GetSceneManager()->GetSceneList();
}

void SceneManagerWindow::DoInterface()
{
	SceneManager* sceneManager = Application::GetApplication()->GetSceneManager();

	m_windowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
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

                if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
                {
                    int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                    if (n_next >= 0 && n_next < scenes->size())
                    {
                        scenes->at(n) = scenes->at(n_next);
                        scenes->at(n_next) = item;
                        ImGui::ResetMouseDragDelta();
                    }
                }
            }
            ImGui::TreePop();
        }

        if(ImGui::Button("Revert Scene Order"))
        {
			sceneManager->SetSceneList(m_originalSceneOrder);
		}
        ImGui::SameLine();
        if(ImGui::Button("Save Scene Order"))
		{
			Application::GetApplication()->GetProjectFile()->UpdateProjectFile();
            UpdateOriginalSceneOrder();
		}

        if (selectedItem)
        {
            if (ImGui::Button("Load Scene"))
            {
                sceneManager->LoadScene(scenes->at(m_selectedSceneIndex));
            }
        }

		if (ImGui::Button("Exit"))
		{
			m_uiOpen = false;
		}
	}
	ImGui::End();
}