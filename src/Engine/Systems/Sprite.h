#pragma once

#include "../ECS/Component.hpp"
#include "Transform.h"
#include "../Core/ResourceManager.h"
#include "../Resource/Texture.h"
#include "../Core/Graphics/GraphicsBuffer.h"

#include <glm/glm.hpp>

struct Sprite : public Component
{
	Transform* m_spriteTransform = nullptr;

	bool m_bCreatedDescriptorSets = false;
	std::vector<GraphicsBuffer> m_uniformBuffers;
	std::vector<void*> m_uniformBuffersMapped;
	std::vector<VkDescriptorSet> m_descriptorSets;

	std::shared_ptr<Texture> m_spriteTexture = nullptr;
	glm::vec4 m_colour;

	static bool m_bInitializedSpriteQuad;
	static GraphicsBuffer m_vertexBuffer;
	static GraphicsBuffer m_indexBuffer;
	static std::vector<Sprite> m_componentData;

	nlohmann::json WriteJson()
	{ 
		nlohmann::json data;
		data = { {"spritePath", m_spriteTexture->m_sLocalPath }, {"colour", {m_colour.x, m_colour.y, m_colour.z, m_colour.w}} };

		return data;
	}

	void FromJson(const nlohmann::json& j)
	{
		m_spriteTexture = ResourceManager::LoadResource<Texture>(j["spritePath"]);
		m_colour = glm::vec4(j["colour"][0], j["colour"][1], j["colour"][2], j["colour"][3]);
	}

	void OnInitialize()
	{
		m_colour = glm::vec4(1.0f);
	}

	void OnInitialize(glm::vec4 _colour)
	{
		m_colour = _colour;
	}

	void OnInitialize(std::string _sheetPath, glm::vec4 _colour)
	{
		m_spriteTexture = ResourceManager::LoadResource<Texture>(_sheetPath);
		m_colour = _colour;
	}

	void OnInitialize(std::string _sheetPath)
	{
		m_spriteTexture = ResourceManager::LoadResource<Texture>(_sheetPath);
		m_colour = glm::vec4(1.0f);
	}

	Component* GetAsComponent(std::string entityID) override
	{
		return GetComponent(entityID);
	}

	virtual void AddComponent() override;
	virtual void RemoveComponent(std::string entityID) override;
	virtual void RemoveAllComponents() override;

	Sprite* GetComponent(std::string entityID);

	virtual void Update(bool useThreads, int maxComponentsPerThread) override;
	virtual void ThreadUpdate(int _buffer, int _amount) override;


	virtual void PreRender() override;
	virtual void Render(bool useThreads, int maxComponentsPerThread) override;
	virtual void ThreadRender(int _buffer, int _amount) override;
	virtual void OnRender() override;

	virtual void LoadComponentDataFromJson(nlohmann::json& j) override;
	virtual nlohmann::json WriteComponentDataToJson() override;
};
