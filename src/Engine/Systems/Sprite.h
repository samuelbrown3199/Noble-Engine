#include "../ECS/Component.hpp"
#include "Transform.h"
#include "../Core/ResourceManager.h"
#include "../Resource/Texture.h"

#include <glm/glm.hpp>

struct Sprite : public ComponentData<Sprite>
{
	Transform* m_spriteTransform = nullptr;

	std::shared_ptr<Texture> m_spriteTexture = nullptr;
	glm::vec4 m_colour;

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
};
