#pragma once
#ifndef EDITOR_H_
#define EDITOR_H_

#include <Windows.h>

#include <Engine\Useful.h>
#include <Engine/imgui/imgui.h>
#include <Engine/imgui/backends/imgui_impl_sdl2.h>
#include <Engine/imgui/backends/imgui_impl_opengl3.h>

#include <Engine\Core\Application.h>
#include <Engine\Core\Renderer.h>
#include <Engine\Core\SceneManager.h>

class EditorUI : public DebugUI
{
	bool show_demo_window = false;
	std::map<int, GLenum> m_mRenderModes;

public:

	void InitializeInterface() override
	{
		m_mRenderModes[0] = GL_TRIANGLES;
		m_mRenderModes[1] = GL_LINES;

		m_windowFlags |= ImGuiWindowFlags_NoMove;
		m_windowFlags |= ImGuiWindowFlags_NoTitleBar;
		m_windowFlags |= ImGuiWindowFlags_NoResize;
	}

	void DoInterface() override
	{
		DoMainMenuBar();

		ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(300, Renderer::GetScreenSize().y));

		ImGui::Begin("Editor", &m_uiOpen, m_windowFlags);

		if(ImGui::Button("Create Entity"))
		{
			Application::CreateEntity();
		}

		std::vector<Entity>& entities = Application::GetEntityList();
		if (ImGui::BeginListBox("Entities"))
		{
			static int selItem;
			for (int i = 0; i < entities.size(); i++)
			{
				const bool is_selected = (selItem == i);
				if (ImGui::Selectable(entities.at(i).m_sEntityName.c_str(), is_selected))
					selItem = i;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndListBox();
		}

		ImGui::End();

		if (show_demo_window)
			ImGui::ShowDemoWindow();
	}

	void DoMainMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				DoFileMenu();
				ImGui::EndMenu();
			}

#ifndef NDEBUG
			if (ImGui::BeginMenu("Developer Debug"))
			{
				DoDebugMenu();
				ImGui::EndMenu();
			}
#endif

			ImGui::EndMainMenuBar();
		}
	}

	void DoFileMenu()
	{
		ImGui::MenuItem("File", NULL, false, false);
		if (ImGui::MenuItem("New Scene"))
		{
			Application::ClearLoadedScene();
		}
		if (ImGui::BeginMenu("Open Scene"))
		{
			std::vector<std::string> scenes = GetAllFilesOfType(GetGameDataFolder(), ".nsc");
			for (int n = 0; n < scenes.size(); n++)
			{
				if (ImGui::Button(GetFolderLocationRelativeToGameData(scenes.at(n)).c_str()))
				{
					SceneManager::LoadScene(GetFolderLocationRelativeToGameData(scenes.at(n)));
				}
			}

			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Save Scene"))
		{
			SceneManager::SaveScene("GameData\\Scenes\\TestScene.nsc");
		}
	}

	void DoDebugMenu()
	{
		ImGui::Checkbox("Demo Window", &show_demo_window);
	}
};

#endif