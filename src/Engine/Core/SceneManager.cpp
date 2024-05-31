#include "SceneManager.h"

#include "Application.h"
#include "Registry.h"
#include "ResourceManager.h"
#include "../Core/EngineResources\Scene.h"
#include "ProjectFile.h"

#include "../imgui/imgui.h"

#include <unordered_map>

void SceneManager::ClearLoadedScene()
{
	m_currentScene = nullptr;
	Application::GetApplication()->ClearLoadedScene();
}

void SceneManager::LoadScene(int sceneIndex)
{
	if (sceneIndex < 0 || sceneIndex >= m_vScenes.size())
	{
		LogError("Scene index out of range");
		return;
	}

	m_vKeysToCheck.clear();
	m_vKeysToCheck.push_back("LightingSettings");
	m_vKeysToCheck.push_back("Entities");
	m_vKeysToCheck.push_back("ComponentData");

	Application::GetApplication()->ClearLoadedScene();
	m_currentScene = std::make_shared<Scene>(m_vScenes[sceneIndex]);
	m_currentScene->OnLoad();
	m_currentScene->LoadSceneIntoApplication();
}

void SceneManager::LoadScene(std::string scenePath)
{
	int sceneIndex= GetSceneIndex(scenePath);
	if (sceneIndex == -1)
		return;

	LoadScene(sceneIndex);
}

void SceneManager::CreateNewScene(std::string sceneName)
{
	if (Application::GetApplication()->GetPlayMode())
		return;

	std::string scenePath = GetGameDataFolder() + "\\" + sceneName + ".nsc";
	std::string relativePath = GetFolderLocationRelativeToGameData(scenePath);

	SaveScene(scenePath);

	m_vScenes.push_back(relativePath);
	Application::GetApplication()->GetProjectFile()->UpdateProjectFile();
}

void SceneManager::SaveLoadedScene()
{
	if (m_currentScene == nullptr)
		return;

	SaveScene(m_currentScene->GetResourcePath());
	m_currentScene->OnLoad();

	Application::GetApplication()->GetProjectFile()->UpdateProjectFile();
}

void SceneManager::SaveScene(std::string scenePath)
{
	if (Application::GetApplication()->GetPlayMode())
		return;

	if (scenePath.empty())
		return;

	Renderer* renderer = Application::GetApplication()->GetRenderer();

	LogInfo("Saving Scene " + scenePath);

	std::unordered_map<std::string, Entity>& entities = Application::GetApplication()->GetEntityList();

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = registry->GetComponentRegistry();

	nlohmann::json data;
	AddVersionDataToJson(data);

	glm::vec3 clearColour = renderer->GetClearColour();
	data["LightingSettings"]["ClearColour"] = { clearColour.x, clearColour.y, clearColour.z };
	data["LightingSettings"]["AmbientColour"] = { renderer->m_sceneData.ambientColour.x, renderer->m_sceneData.ambientColour.y , renderer->m_sceneData.ambientColour.z , renderer->m_sceneData.ambientColour.w };

	std::unordered_map<std::string, Entity>::iterator it;
	for (it = entities.begin(); it != entities.end(); it++)
	{
		if (it->second.m_bAvailableForUse)
			continue;

		data["Entities"][it->second.m_sEntityID]["EntityName"] = it->second.m_sEntityName;
		data["Entities"][it->second.m_sEntityID]["ParentID"] = it->second.m_sEntityParentID;
	}
	for (int i = 0; i < compRegistry->size(); i++)
	{
		data["ComponentData"][compRegistry->at(i).first] = compRegistry->at(i).second.m_comp->WriteComponentDataToJson();
	}
	
	CameraBase* cam = renderer->GetCamera();
	if(cam != nullptr)
	{
		if (cam->m_state == CameraState::editorCam)
		{
			data["EditorCamera"]["Position"] = { cam->GetPosition().x,cam->GetPosition().y,cam->GetPosition().z };
			data["EditorCamera"]["Rotation"] = { cam->GetRotation().x,cam->GetRotation().y,cam->GetRotation().z };
		}
	}

	std::fstream sceneFile(scenePath, 'w');
	sceneFile << data.dump();
	sceneFile.close();

	Application::GetApplication()->LinkChildEntities();

	LogInfo("Saved Scene " + scenePath);
}

void SceneManager::LoadDefaultScene()
{
	if (m_vScenes.size() > 0)
	{
		LoadScene(0);
	}
}

void SceneManager::LoadSceneDatabase(nlohmann::json sceneDatabase)
{
	m_vScenes.clear();

	for (auto it : sceneDatabase.items())
	{
		m_vScenes.push_back(sceneDatabase[it.key()]["ScenePath"]);
	}
}

nlohmann::json SceneManager::WriteSceneDatabase()
{
	nlohmann::json data;

	for (int i = 0; i < m_vScenes.size(); i++)
	{
		data[std::to_string(i)]["ScenePath"] = m_vScenes.at(i);
	}

	return data;
}

int SceneManager::GetSceneIndex(std::string scenePath)
{
	for(int i = 0; i < m_vScenes.size(); i++)
	{
		if (m_vScenes[i] == scenePath)
			return i;
	}

	LogError(FormatString("Scene %s not found in scene database", scenePath.c_str()));
	return -1;
}

std::string SceneManager::GetCurrentSceneLocalPath()
{
	if (m_currentScene)
		return GetFolderLocationRelativeToGameData(m_currentScene->GetResourcePath());
	else
		return "";
}