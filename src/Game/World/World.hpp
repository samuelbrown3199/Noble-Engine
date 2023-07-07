#pragma once
#ifndef WORLD_HPP
#define WORLD_HPP

#include <Engine/ECS/Behaviour.hpp>
#include <Engine/Core/Logger.h>
#include <Engine/Core/Application.h>
#include <Engine/Useful.h>
#include <Engine/Systems/MeshRenderer.h>
#include <Engine/Systems/AudioListener.h>

#include <vector>

struct World : public Behaviour
{
	std::string m_sWorldName;

	glm::vec2 m_planetScale;

	void GenerateBackgroundPlanet()
	{
		m_planetScale = glm::vec2(500, 500);

		Entity* planetSpawner;

		planetSpawner = Application::CreateEntity();
		planetSpawner->AddComponent<Transform>(glm::vec3(0, 0, -13), glm::vec3(0, 0, 0), glm::vec3(m_planetScale.x*4, m_planetScale.y * 4, 1));
		planetSpawner->AddComponent<Sprite>("GameData\\Textures\\SpaceBackground1.png", glm::vec4(1,1,1, 0.35f));
		planetSpawner->AddComponent<AudioListener>();

		planetSpawner = Application::CreateEntity();
		planetSpawner->AddComponent<Transform>(glm::vec3(0, 0, -12.9), glm::vec3(0, 0, 0), glm::vec3(m_planetScale.x * 4, m_planetScale.y * 4, 1));
		planetSpawner->AddComponent<Sprite>("GameData\\Textures\\Stars1.png");

		planetSpawner = Application::CreateEntity();
		planetSpawner->AddComponent<Transform>(glm::vec3(0, 0, -12), glm::vec3(0, 0, 0), glm::vec3(m_planetScale.x, m_planetScale.y, 1));
		planetSpawner->AddComponent<Sprite>("GameData\\Textures\\PlanetBase.png", glm::vec4(1, 1, 1, 1));

		planetSpawner = Application::CreateEntity();
		planetSpawner->AddComponent<Transform>(glm::vec3(0, 0, -11.9), glm::vec3(0, 0, 0), glm::vec3(m_planetScale.x, m_planetScale.y, 1));
		planetSpawner->AddComponent<Sprite>("GameData\\Textures\\Continents1.png", glm::vec4(0, 1, 0, 1));

		planetSpawner = Application::CreateEntity();
		planetSpawner->AddComponent<Transform>(glm::vec3(5, 0, 15), glm::vec3(0, 0, 0), glm::vec3(1,1,1));
		MeshRenderer* mr = planetSpawner->AddComponent<MeshRenderer>();

		AudioSource* aSource = planetSpawner->AddComponent<AudioSource>("GameData\\Sounds\\glass_smash.ogg", "Master", false);
		aSource->m_b3DSound = true;
		aSource->m_iLoopCount = -1;

		mr->m_model = ResourceManager::LoadResource<Model>("GameData\\Models\\cottage_obj.obj");
		mr->m_texture = ResourceManager::LoadResource<Texture>("GameData\\Textures\\cottage_diffuse.png");
		mr->m_colour = glm::vec4(1, 1, 1, 1);
	}

	void Start()
	{
		Logger::LogInformation("Initializing World!");

		GenerateBackgroundPlanet();
	}

	void Update()
	{

	}
};

#endif