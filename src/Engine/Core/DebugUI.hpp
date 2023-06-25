#pragma once
#ifndef DEBUGUI_H_
#define DEBUGUI_H_

#include "../Useful.h"
#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../imgui/backends/imgui_impl_opengl3.h"

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

#endif