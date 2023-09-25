#include "Sprite.h"
#include "../Core/Application.h"
#include "../ECS/Entity.hpp"
#include "../Core/Graphics/Renderer.h"

GLuint Sprite::m_iQuadVAO;
bool Sprite::m_bInitializedSpriteQuad = false;

std::vector<Vertex> Sprite::vertices =
{
	{{-0.5f, -0.5f, 0.0}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

std::vector<glm::vec3> Sprite::boundingBox =
{
	glm::vec3(- 0.5f, -0.5f, 0.0),
	glm::vec3(0.5f, -0.5f, 0.0),
	glm::vec3(0.5f, 0.5f, 0.0),
	glm::vec3(-0.5f, 0.5f, 0.0)
};

const std::vector<uint32_t> indices =
{
	0, 1, 2, 2, 3, 0
};

void Sprite::PreRender()
{
	if (!m_bInitializedSpriteQuad)
	{
		glGenVertexArrays(1, &m_iQuadVAO);
		glBindVertexArray(m_iQuadVAO);

		unsigned int VBO, EBO;
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// normals attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		m_bInitializedSpriteQuad = true;
	}
}

void Sprite::OnRender()
{
	if (m_spriteTexture == nullptr)
		return;

	if (m_shader == nullptr)
		return;

	m_vertices = &vertices;
	m_boundingBox = &boundingBox;

	Renderable::OnRender();
	if (!m_bOnScreen)
		return;

	glBindVertexArray(m_iQuadVAO);

	glm::vec3 scale = m_transform->m_scale;
	float heightRat = (float)m_spriteTexture->m_iWidth / (float)m_spriteTexture->m_iHeight;
	m_transform->m_scale = glm::normalize(glm::vec3(m_spriteTexture->m_iWidth * heightRat, m_spriteTexture->m_iHeight * heightRat, scale.z));
	m_transform->m_scale.x = scale.x;
	m_transform->m_scale.y = scale.y;

	m_shader->UseProgram();
	glBindTexture(GL_TEXTURE_2D, m_spriteTexture->m_iTextureID);

	m_shader->BindMat4("transMat", m_transform->m_transformMat);
	m_shader->BindVector4("colour", m_colour);

	glDrawElements(Renderer::GetRenderMode(), 6, GL_UNSIGNED_INT, 0);
}