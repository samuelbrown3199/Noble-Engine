#pragma once
#ifndef EDITOR_H_
#define EDITOR_H_

#include <Engine\Useful.h>
#include <Engine/imgui/imgui.h>
#include <Engine/imgui/backends/imgui_impl_sdl2.h>
#include <Engine/imgui/backends/imgui_impl_opengl3.h>

#include <Engine\Core\Application.h>
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
	}

	void DoInterface() override
	{
		ImGui::Begin("Editor", &m_uiOpen, m_windowFlags);

		ImGui::Checkbox("Demo Window", &show_demo_window);

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

		if (ImGui::Button("Delete Scene"))
			SceneManager::ClearLoadedScene();

		if (ImGui::Button("Switch Scene"))
			SceneManager::LoadScene("GameData\\Scenes\\TestSaveScene.json");

		if (ImGui::Button("Save Scene"))
			SceneManager::SaveScene("GameData\\Scenes\\TestSaveScene.json");

		ImGui::End();


		if (show_demo_window)
			ImGui::ShowDemoWindow();
	}
};

#endif