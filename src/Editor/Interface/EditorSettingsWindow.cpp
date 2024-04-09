#include "EditorSettingsWindow.h"

void EditorSettingsWindow::InitializeInterface()
{
}

void EditorSettingsWindow::DoInterface()
{
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::Begin("Settings", &m_uiOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

	ImGui::Text("Settings, NYI");

	ImGui::End();
}