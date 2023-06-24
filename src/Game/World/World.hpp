#pragma once
#ifndef WORLD_HPP
#define WORLD_HPP

#include <Engine/ECS/Behaviour.hpp>
#include <Engine/Core/Logger.h>
#include <Engine/Core/Application.h>
#include <Engine/Useful.h>
#include <Engine/Systems/MeshRenderer.h>

#include <vector>

#include "TileChunk.h"

struct World : public Behaviour
{
	std::string m_sWorldName;

	glm::vec3 m_worldAmbientColor;
	float m_fWorldAmbientStrength;
	std::vector<TileChunk*> m_vWorldChunks;

	glm::vec2 m_planetScale;

	TileChunk* GetChunkFromWorldPos(const glm::vec2& _worldPos)
	{
		int chunkWidth = TileChunk::m_iChunkWidth * 10;
		int xPos = _worldPos.x;
		int yPos = _worldPos.y;
		int roundedX = ((xPos % chunkWidth) > chunkWidth / 2) ? xPos + chunkWidth - (xPos % chunkWidth) : xPos - (xPos % chunkWidth);
		int roundedY = ((yPos % chunkWidth) > chunkWidth / 2) ? yPos + chunkWidth - (yPos % chunkWidth) : yPos - (yPos % chunkWidth);
		glm::ivec2 roundedPosition = glm::vec2(roundedX, roundedY);


		std::string formattedString = FormatString("From world pos (%d,%d) we looked for chunk pos (%d,%d)", xPos, yPos, roundedPosition.x, roundedPosition.y);
		std::cout << formattedString << std::endl;

		for (int i = 0; i < m_vWorldChunks.size(); i++)
		{
			TileChunk* targetChunk = m_vWorldChunks.at(i);
			if (!targetChunk->m_chunkTransform)
				continue;
			if (targetChunk->m_chunkTransform->m_position.x == roundedPosition.x && targetChunk->m_chunkTransform->m_position.y == roundedPosition.y)
			{
				std::cout << "Found chunk!" << std::endl;
				return targetChunk;
			}
		}

		return nullptr;
	}

	void GenerateBackgroundPlanet()
	{
		m_worldAmbientColor = glm::vec3(1, 1, 1);
		m_fWorldAmbientStrength = 1.0f;
		m_planetScale = glm::vec2(500, 500);

		/*Entity* planetSpawner;

		planetSpawner = Application::CreateEntity();
		planetSpawner->AddComponent<Transform>(glm::vec3(0, 0, -13), glm::vec3(0, 0, 0), glm::vec3(m_planetScale.x*4, m_planetScale.y * 4, 1));
		planetSpawner->AddComponent<Sprite>("GameData\\Textures\\SpaceBackground1.png", glm::vec4(1,1,1, 0.35f));

		planetSpawner = Application::CreateEntity();
		planetSpawner->AddComponent<Transform>(glm::vec3(0, 0, -12.9), glm::vec3(0, 0, 0), glm::vec3(m_planetScale.x * 4, m_planetScale.y * 4, 1));
		planetSpawner->AddComponent<Sprite>("GameData\\Textures\\Stars1.png");

		planetSpawner = Application::CreateEntity();
		planetSpawner->AddComponent<Transform>(glm::vec3(0, 0, -12), glm::vec3(0, 0, 0), glm::vec3(m_planetScale.x, m_planetScale.y, 1));
		planetSpawner->AddComponent<Sprite>("GameData\\Textures\\PlanetBase.png", glm::vec4(0, 0, 1, 1));

		planetSpawner = Application::CreateEntity();
		planetSpawner->AddComponent<Transform>(glm::vec3(0, 0, -11.9), glm::vec3(0, 0, 0), glm::vec3(m_planetScale.x, m_planetScale.y, 1));
		planetSpawner->AddComponent<Sprite>("GameData\\Textures\\Continents1.png", glm::vec4(0, 1, 0, 1));

		planetSpawner = Application::CreateEntity();
		planetSpawner->AddComponent<Transform>(glm::vec3(5, 0, 15), glm::vec3(0, 0, 0), glm::vec3(1,1,1));
		MeshRenderer* mr = planetSpawner->AddComponent<MeshRenderer>();

		planetSpawner->AddComponent<AudioSource>("GameData\\Sounds\\glass_smash.ogg", "Master")->m_bPlay = false;

		mr->m_model = ResourceManager::LoadResource<Model>("GameData\\Models\\cottage_obj.obj");
		mr->m_texture = ResourceManager::LoadResource<Texture>("GameData\\Textures\\cottage_diffuse.png");
		mr->m_colour = glm::vec4(1, 1, 1, 1);*/
	}

	void Start()
	{
		Logger::LogInformation("Initializing World!");

		GenerateBackgroundPlanet();
	}

	void Update()
	{
		Application::m_mainShaderProgram->BindFloat("ambientStrength", m_fWorldAmbientStrength);
		Application::m_mainShaderProgram->BindVector3("ambientColour", m_worldAmbientColor);
	}
};

#endif