#include "Sprite.h"
#include "../Application.h"
#include "../Graphics/Renderer.h"
#include "../ECS/Entity.hpp"

std::vector<uint32_t> Sprite::spriteQuadIndices =
{
	0, 1, 2, 2, 3, 0
};
std::vector<Vertex> Sprite::spriteQuadVertices =
{
	Vertex(glm::vec3{ 0.0f, 0.0f, 0.0 }, glm::vec2{ 0.0f, 0.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }),
	Vertex(glm::vec3{ 1.0f, 0.0f, 0.0 }, glm::vec2{ 1.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }),
	Vertex(glm::vec3{ 1.0f, 1.0f, 0.0 }, glm::vec2{ 1.0f, 1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }),
	Vertex(glm::vec3{ 0.0f, 1.0f, 0.0 }, glm::vec2{ 0.0f, 1.0f }, glm::vec3{ 1.0f, 1.0f, 1.0f })
};
std::vector<glm::vec3> Sprite::boundingBox =
{
	glm::vec3(0.0f, 0.0f, 0.0),
	glm::vec3(1.0f, 0.0f, 0.0),
	glm::vec3(1.0f, 1.0f, 0.0),
	glm::vec3(0.0f, 1.0f, 0.0),
};

void Sprite::OnPreRender()
{
	Renderer* renderer = Application::GetRenderer();

	m_meshBuffers = renderer->GetSpriteQuadBuffer();
	m_vertices = &spriteQuadVertices;
	m_boundingBox = &boundingBox;
	m_indices = &spriteQuadIndices;

	Renderable::OnPreRender();
}