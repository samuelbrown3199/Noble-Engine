#include "Sprite.h"
#include "../Core/Application.h"
#include "../Core/Graphics/Renderer.h"
#include "../ECS/Entity.hpp"

GLuint Sprite::m_iQuadVAO;
bool Sprite::m_bInitializedSpriteQuad = false;

ComponentDatalist<Sprite> Sprite::m_componentList;

void Sprite::AddComponent()
{
	m_componentList.AddComponent(this);
}

void Sprite::AddComponentToEntity(std::string entityID)
{
	m_componentList.AddComponentToEntity(entityID);
	Application::GetEntity(entityID)->GetAllComponents();
}

void Sprite::RemoveComponent(std::string entityID)
{
	m_componentList.RemoveComponent(entityID);
}

void Sprite::RemoveAllComponents()
{
	m_componentList.RemoveAllComponents();
	m_bInitializedSpriteQuad = false;
}

Sprite* Sprite::GetComponent(std::string entityID)
{
	return m_componentList.GetComponent(entityID);
}

void Sprite::Update(bool useThreads, int maxComponentsPerThread) {}

void Sprite::PreRender()
{
	if (!m_bInitializedSpriteQuad)
	{
		const std::vector<Vertex> vertices =
		{
			{{-0.5f, -0.5f, 0.0}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
		};

		const std::vector<uint32_t> indices =
		{
			0, 1, 2, 2, 3, 0
		};

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

void Sprite::Render(bool useThreads, int maxComponentsPerThread)
{
	m_componentList.Render(useThreads, maxComponentsPerThread);
}

void Sprite::OnRender()
{
	if (m_spriteTransform == nullptr || Application::GetEntitiesDeleted())
	{
		m_spriteTransform = Application::GetEntity(m_sEntityID)->GetComponent<Transform>();
		return;
	}
	if (m_spriteTexture == nullptr)
		return;

	//if (m_shader == nullptr)
		//return;

	glBindVertexArray(m_iQuadVAO);

	glm::vec3 scale = m_spriteTransform->m_scale;
	float heightRat = (float)m_spriteTexture->m_iWidth / (float)m_spriteTexture->m_iHeight;
	m_spriteTransform->m_scale = glm::normalize(glm::vec3(m_spriteTexture->m_iWidth * heightRat, m_spriteTexture->m_iHeight * heightRat, scale.z));
	m_spriteTransform->m_scale.x = scale.x;
	m_spriteTransform->m_scale.y = scale.y;

	Application::m_mainShaderProgram->UseProgram();
	glm::mat4 finalMat = Renderer::GenerateProjMatrix() * Renderer::GenerateViewMatrix();
	glBindTexture(GL_TEXTURE_2D, m_spriteTexture->m_iTextureID);

	Application::m_mainShaderProgram->BindMat4("vpMat", finalMat);
	Application::m_mainShaderProgram->BindMat4("transMat", m_spriteTransform->m_transformMat);
	Application::m_mainShaderProgram->BindVector4("colour", m_colour);

	glDrawElements(Renderer::GetRenderMode(), 6, GL_UNSIGNED_INT, 0);
}

void Sprite::LoadComponentDataFromJson(nlohmann::json& j)
{
	m_componentList.LoadComponentDataFromJson(j);
}

nlohmann::json Sprite::WriteComponentDataToJson()
{
	return m_componentList.WriteComponentDataToJson();
}