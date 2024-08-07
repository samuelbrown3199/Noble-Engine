#include "Scene.h"

#include "..\..\Core\Registry.h"
#include "..\..\Core\Logger.h"
#include "..\..\Core\Application.h"
#include "..\..\Core\SceneManager.h"
#include "..\..\Useful.h"

#include <fstream>

Scene::Scene(std::string path)
{
	m_sResourcePath = GetGameFolder() + path;
}

void Scene::OnLoad()
{
	m_sceneData.clear();

	std::ifstream sceneFile(m_sResourcePath);
	m_sceneData = nlohmann::json::parse(sceneFile);
	sceneFile.close();

	std::vector<std::string> keysToCheck = Application::GetApplication()->GetSceneManager()->m_vKeysToCheck;

	for (int i = 0; i < keysToCheck.size(); i++)
	{
		auto check = m_sceneData.find(keysToCheck.at(i));
		if (check == m_sceneData.end())
		{
			LogError(FormatString("Scene file %s is malformed, missing %s information. ", m_sResourcePath.c_str(), keysToCheck.at(i).c_str()));
			return;
		}
	}

	m_bSceneReady = true;
}

void Scene::LoadSceneIntoApplication()
{
	if (m_bSceneReady == false)
		return;

	Renderer* renderer = Application::GetApplication()->GetRenderer();

	//parse out JSON here. Should probably check that the file isnt malformed in some way as well.
	if (m_sceneData.find("ComponentData") != m_sceneData.end())
	{
		NobleRegistry* registry = Application::GetApplication()->GetRegistry();
		std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = registry->GetComponentRegistry();

		nlohmann::json componentData = m_sceneData.at("ComponentData");

		for (auto it : componentData.items())
		{
			for (int i = 0; i < compRegistry->size(); i++)
			{
				if (compRegistry->at(i).first == it.key())
				{
					compRegistry->at(i).second.m_comp->LoadComponentDataFromJson(componentData[it.key()]);
				}
			}
		}
	}
	if (m_sceneData.find("Entities") != m_sceneData.end())
	{
		nlohmann::json entities = m_sceneData.at("Entities");

		for (auto it : entities.items())
		{
			Entity* ent = Application::GetApplication()->CreateEntity(it.key(), entities[it.key()]["EntityName"], entities[it.key()]["ParentID"]);
			ent->GetAllComponents();
		}

		Application::GetApplication()->LinkChildEntities();
	}
	if (m_sceneData.find("LightingSettings") != m_sceneData.end())
	{
		nlohmann::json lightingInfo = m_sceneData.at("LightingSettings");

		glm::vec3 clearColour = glm::vec3(lightingInfo["ClearColour"][0], lightingInfo["ClearColour"][1], lightingInfo["ClearColour"][2]);
		Application::GetApplication()->GetRenderer()->SetClearColour(clearColour);

		if (lightingInfo.find("AmbientColour") != lightingInfo.end())
		{
			glm::vec4 ambientColour = glm::vec4(lightingInfo["AmbientColour"][0], lightingInfo["AmbientColour"][1], lightingInfo["AmbientColour"][2], lightingInfo["AmbientColour"][3]);
			renderer->m_sceneData.ambientColour = ambientColour;
		}
	}

	if (m_sceneData.find("EditorCamera") != m_sceneData.end())
	{
		CameraBase* cam = renderer->GetCamera();
		if (cam != nullptr && cam->m_state == editorCam)
		{
			cam->SetPosition(glm::vec3(m_sceneData["EditorCamera"]["Position"][0], m_sceneData["EditorCamera"]["Position"][1], m_sceneData["EditorCamera"]["Position"][2]));
			cam->SetRotation(glm::vec3(m_sceneData["EditorCamera"]["Rotation"][0], m_sceneData["EditorCamera"]["Rotation"][1], m_sceneData["EditorCamera"]["Rotation"][2]));
		}
	}
}