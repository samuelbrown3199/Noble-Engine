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

	bool m_bShowSceneSelection = false;

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
		if(m_bShowSceneSelection)
			SelectSceneWindow();

		ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(300, Renderer::GetScreenSize().y));

		ImGui::Begin("Editor", &m_uiOpen, m_windowFlags);

		const char* items[] = { "GL Triangles", "GL Lines" };
		static int item_current = 0;
		ImGui::Combo("Draw Mode", &item_current, items, IM_ARRAYSIZE(items));
		Renderer::SetRenderMode(m_mRenderModes[item_current]);

		float fov = Renderer::GetFov();
		ImGui::SliderFloat("FoV", &fov, 10.0f, 150.0f, FormatString("%2f", fov).c_str());
		Renderer::SetFov(fov);

		float clearColour[3];
		ImGui::ColorEdit3("Clear Buffer Colour", clearColour);
		if (ImGui::Button("Apply"))
			Renderer::SetClearColour(glm::vec3(clearColour[0], clearColour[1], clearColour[2]));

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
		if (ImGui::MenuItem("Open Scene"))
		{
			m_bShowSceneSelection = true;
		}
		if (ImGui::MenuItem("Save Scene"))
		{
			SceneManager::SaveScene("GameData\\Scenes\\TestScene.json");
		}
	}

	void DoDebugMenu()
	{
		ImGui::Checkbox("Demo Window", &show_demo_window);
	}
	
	void SelectSceneWindow()
	{
		std::string cwd = GetWorkingDirectory();
		std::vector<std::string> sceneFiles = GetAllFilesOfType(cwd + "\\GameData", ".json");

		ImGui::SetNextWindowPos(ImVec2(300, 20), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(500, 500));
		ImGui::Begin("Editor", &m_bShowSceneSelection, m_windowFlags);



		ImGui::End();
	}
};

#endif