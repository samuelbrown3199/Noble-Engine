#include "ProjectFile.h"

#include <Engine/Core/Application.h>
#include <Engine/Useful.h>
#include <Engine/Core/ResourceManager.h>
#include "SceneManager.h"

void ProjectFile::CreateProjectFile(std::string projectName, std::string projectDirectory, std::string projectFilePath)
{
	std::shared_ptr<Application> app = Application::GetApplication();

	LoadProjectFile(projectFilePath);

	m_sProjectName = projectName;
	m_sProjectDirectory = projectDirectory;
	m_sProjectFilePath = projectFilePath;
	
	UpdateProjectFile();
}

void ProjectFile::LoadProjectFile(const std::string& file)
{
	std::unique_lock<std::mutex> lock(m_projectFileMutex);

	Application::GetApplication()->ClearLoadedScene();
	ResourceManager* rManager = Application::GetApplication()->GetResourceManager();
	rManager->UnloadAllResources();

	Application::GetApplication()->ResetRegistries();

	std::ifstream projectFile;
	projectFile.open(file);
	m_projectData = nlohmann::json::parse(projectFile);
	projectFile.close();

	LogInfo("Loaded project file " + file);

	if (m_projectData.find("ProjectDetails") != m_projectData.end())
	{
		m_sProjectName = m_projectData["ProjectDetails"]["ProjectName"];
		m_sProjectDirectory = m_projectData["ProjectDetails"]["ProjectDirectory"];
		m_sProjectEngineVersion = m_projectData["ProjectDetails"]["EngineVersion"];

		if(m_sProjectEngineVersion != GetVersionInfoString())
			LogError("Engine has been updated since this project was created. A file update may be required.");
	}
	else
	{
		LogFatalError("Project file is malformed, missing ProjectDetails information.");
		return;
	}

	m_sProjectFilePath = file;

	rManager->SetWorkingDirectory(m_sProjectDirectory);
	std::string gamedataPath = m_sProjectDirectory + "\\GameData";
	if(!PathExists(gamedataPath))
		CreateNewDirectory(gamedataPath);

	if (m_projectData.find("Resources") != m_projectData.end())
		rManager->LoadResourceDatabase(m_projectData.at("Resources"));
	else
		LogFatalError("Project file is malformed, missing Resources information.");
	rManager->CompileShaders();

	if(m_projectData.find("Scenes") != m_projectData.end())
		Application::GetApplication()->GetSceneManager()->LoadSceneDatabase(m_projectData.at("Scenes"));
	else
		LogFatalError("Project file is malformed, missing Scenes information.");
}

void ProjectFile::UpdateProjectFile()
{
	std::unique_lock<std::mutex> lock(m_projectFileMutex);

	std::shared_ptr<Application> app = Application::GetApplication();

	m_projectData["ProjectDetails"]["ProjectName"] = m_sProjectName;
	m_projectData["ProjectDetails"]["ProjectDirectory"] = m_sProjectDirectory;
	m_projectData["ProjectDetails"]["EngineVersion"] = GetVersionInfoString();

	m_projectData["Resources"] = app->GetResourceManager()->WriteResourceDatabase();
	m_projectData["Scenes"] = app->GetSceneManager()->WriteSceneDatabase();

	app->GetResourceManager()->LoadResourceDatabase(m_projectData.at("Resources"));

	std::fstream file(m_sProjectFilePath, 'w');
	file << m_projectData.dump();
	file.close();

	LogInfo("Updated project file " + m_sProjectFilePath);
}

void ProjectFile::UpdateResourceDatabase(nlohmann::json resourceDatabase)
{
	m_projectData["Resources"] = resourceDatabase;

	std::fstream file(m_sProjectFilePath, 'w');
	file << m_projectData.dump();
	file.close();
}