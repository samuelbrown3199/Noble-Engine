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

	nlohmann::json m_projectData;

	static bool CreateProjectFile(std::string projectName, std::string projectDirectory);
	void LoadProjectFile(const std::string& file);
};