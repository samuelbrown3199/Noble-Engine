#include "ProjectFile.h"

#include <Engine/Core/Application.h>
#include <Engine/Useful.h>
#include <Engine/Core/ResourceManager.h>

bool ProjectFile::CreateProjectFile(std::string projectName, std::string projectDirectory)
{
	nlohmann::json data;

	data["ProjectName"] = projectName;
	data["ProjectDirectory"] = projectDirectory;

	std::string projectFileDir = GetWorkingDirectory() + "\\Projects";
	if (!PathExists(projectFileDir))
		CreateNewDirectory(projectFileDir);

	std::string file = projectFileDir + "\\" + projectName + ".npj";
	std::fstream projectFile(file, 'w');

	projectFile << data.dump();
	projectFile.close();

	return true;
}

void ProjectFile::LoadProjectFile(const std::string& file)
{
	Application::ClearLoadedScene();

	std::ifstream projectFile;
	projectFile.open(file);
	m_projectData = nlohmann::json::parse(projectFile);
	projectFile.close();

	Logger::LogInformation("Loaded project file " + file);

	m_sProjectName = m_projectData["ProjectName"];
	m_sProjectDirectory = m_projectData["ProjectDirectory"];

	ResourceManager::SetWorkingDirectory(m_sProjectDirectory);
	std::string gamedataPath = m_sProjectDirectory + "\\GameData";
	if(!PathExists(gamedataPath))
		CreateNewDirectory(gamedataPath);

}