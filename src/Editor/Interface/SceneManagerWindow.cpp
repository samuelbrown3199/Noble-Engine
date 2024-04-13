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
        if (ImGui::TreeNode("Scenes"))
        {  
            std::vector<std::string>* scenes = sceneManager->GetSceneList();
            for (int n = 0; n < scenes->size(); n++)
            {
                std::string item = scenes->at(n);
                ImGui::Selectable(item.c_str());

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

		if (ImGui::Button("Exit"))
		{
			m_uiOpen = false;
		}
	}
	ImGui::End();
}