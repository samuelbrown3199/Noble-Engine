#include "ProjectFile.h"

#include <Engine/Core/Application.h>
#include <Engine/Useful.h>
#include <Engine/Core/ResourceManager.h>
#include "SceneManager.h"

bool ProjectFile::CreateProjectFile(std::string projectName, std::string projectDirectory)
{
	nlohmann::json data;

	data["ProjectDetails"]["ProjectName"] = projectName;
	data["ProjectDetails"]["ProjectDirectory"] = projectDirectory;
	data["ProjectDetails"]["EngineVersion"] = GetVersionInfoString();

	std::string projectFileDir = projectDirectory + "\\" + projectName + ".npj";
	std::fstream projectFile(projectFileDir, 'w');

	projectFile << data.dump();
	projectFile.close();

	return true;
}

void ProjectFile::LoadProjectFile(const std::string& file)
{
	Application::GetApplication()->ClearLoadedScene();
	ResourceManager::UnloadAllResources();

	std::ifstream projectFile;
	projectFile.open(file);
	m_projectData = nlohmann::json::parse(projectFile);
	projectFile.close();

	Logger::LogInformation("Loaded project file " + file);

	if (m_projectData.find("ProjectDetails") != m_projectData.end())
	{
		m_sProjectName = m_projectData["ProjectDetails"]["ProjectName"];
		m_sProjectDirectory = m_projectData["ProjectDetails"]["ProjectDirectory"];
		m_sProjectEngineVersion = m_projectData["ProjectDetails"]["EngineVersion"];

		if(m_sProjectEngineVersion != GetVersionInfoString())
			Logger::LogError("Engine has been updated since this project was created. A file update may be required.", 1);
	}
	else
	{
		Logger::LogError("Project file is malformed, missing ProjectDetails information.", 2);
		return;
	}

	m_sProjectFilePath = file;

	ResourceManager::SetWorkingDirectory(m_sProjectDirectory);
	std::string gamedataPath = m_sProjectDirectory + "\\GameData";
	if(!PathExists(gamedataPath))
		CreateNewDirectory(gamedataPath);

	if (m_projectData.find("Resources") != m_projectData.end())
		ResourceManager::LoadResourceDatabase(m_projectData.at("Resources"));
	else
		Logger::LogError("Project file is malformed, missing Resources information.", 2);

	if(m_projectData.find("Scenes") != m_projectData.end())
		Application::GetApplication()->GetSceneManager()->LoadSceneDatabase(m_projectData.at("Scenes"));
	else
		Logger::LogError("Project file is malformed, missing Scenes information.", 2);
}

void ProjectFile::UpdateProjectFile()
{
	std::shared_ptr<Application> app = Application::GetApplication();

	m_projectData["ProjectDetails"]["ProjectName"] = m_sProjectName;
	m_projectData["ProjectDetails"]["ProjectDirectory"] = m_sProjectDirectory;
	m_projectData["ProjectDetails"]["EngineVersion"] = GetVersionInfoString();

	m_projectData["Resources"] = app->GetResourceManager()->WriteResourceDatabase();
	m_projectData["Scenes"] = app->GetSceneManager()->WriteSceneDatabase();

	std::fstream file(m_sProjectFilePath, 'w');
	file << m_projectData.dump();
	file.close();
}

void ProjectFile::UpdateResourceDatabase(nlohmann::json resourceDatabase)
{
	m_projectData["Resources"] = resourceDatabase;

	std::fstream file(m_sProjectFilePath, 'w');
	file << m_projectData.dump();
	file.close();
}