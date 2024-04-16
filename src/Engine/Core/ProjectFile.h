#pragma once

#include <Engine/Useful.h>
#include <nlohmann/json.hpp>

#include <fstream>
#include <string>

class ProjectFile
{
public:

	std::string m_sProjectName;
	std::string m_sProjectDirectory;
	std::string m_sProjectEngineVersion;

	std::string m_sProjectFilePath;

	nlohmann::json m_projectData;

	void CreateProjectFile(std::string projectName, std::string projectDirectory, std::string projectFilePath);
	void LoadProjectFile(const std::string& file);
	void UpdateProjectFile();

	std::string GetProjectFilePath() { return m_sProjectFilePath; }

	void UpdateResourceDatabase(nlohmann::json resourceDatabase);
};