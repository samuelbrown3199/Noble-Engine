#pragma once
#ifndef DEBUGUI_H_
#define DEBUGUI_H_

#include "../Useful.h"
#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../imgui/backends/imgui_impl_opengl3.h"

#include "..\Core\Application.h"
#include "..\Core\SceneManager.h"

class DebugUI
{
public:

	bool m_uiOpen = false;
	ImGuiWindowFlags m_windowFlags = 0;

	virtual void InitializeInterface() {}
	virtual void DoInterface() = 0;
};


class TestDebugUI : public DebugUI
{

	float f = 0.0f;
	int counter = 0;

public:

	void DoInterface() override
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}
};

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