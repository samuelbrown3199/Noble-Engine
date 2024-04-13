#include "EditorSettingsWindow.h"

void EditorSettingsWindow::InitializeInterface(ImGuiWindowFlags defaultFlags)
{
	EditorToolUI::InitializeInterface(defaultFlags);
}

void EditorSettingsWindow::DoInterface()
{
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::Begin("Settings", &m_uiOpen, m_windowFlags);

	ImGui::Text("Settings, NYI");

	ImGui::End();
}