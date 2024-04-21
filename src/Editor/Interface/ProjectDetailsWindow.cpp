#include "ProjectDetailsWindow.h"
#include "../EditorManagement/EditorManager.h"

void ProjectDetailsWindow::InitializeInterface(ImGuiWindowFlags defaultFlags)
{
	EditorToolUI::InitializeInterface(defaultFlags);
}

void ProjectDetailsWindow::DoInterface()
{
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::Begin("Project Details", &m_uiOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

	UpdateWindowState();

	if (m_pProjectFile == nullptr)
	{
		ImGui::Text("How did you get here?!");
		ImGui::End();
		return;
	}

	ImGui::InputText("Project Name", &m_pProjectFile->m_sProjectName);

	std::string projectPath = "Project Directory: " + m_pProjectFile->m_sProjectDirectory;
	ImGui::Text(projectPath.c_str());

	//if (ImGui::Button("Browse For Directory"))
	//{
	//	IGFD::FileDialogConfig config;
	//	config.path = m_pProjectFile->m_sProjectDirectory;
	//	ImGuiFileDialog::Instance()->OpenDialog("ChooseDirectory", "Choose Project Directory", nullptr, config);
	//}

	ImGui::End();

	//if (ImGuiFileDialog::Instance()->Display("ChooseDirectory"))
	//{
	//	if (ImGuiFileDialog::Instance()->IsOk())
	//	{
	//		std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
	//		m_pProjectFile->m_sProjectDirectory = filePath;
	//	}
	//	ImGuiFileDialog::Instance()->Close();
	//}
}

void ProjectDetailsWindow::SetProjectFile(ProjectFile* projectFile)
{
	m_pProjectFile = projectFile;
}