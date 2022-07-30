#include "SpriteSystem.h"
#include "../Core/Application.h"

#include <iostream>

std::weak_ptr<SystemBase> SpriteSystem::self;
std::weak_ptr<SystemBase> Sprite::componentSystem;
std::vector<Sprite> Sprite::componentData;

unsigned int SpriteSystem::m_iQuadVAO;

void SpriteSystem::PreRender()
{
	if (!m_bSetupQuad)
	{
		float vertices[] = {
			// positions          // colors           // texture coords
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
		};
		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};

		glGenVertexArrays(1, &m_iQuadVAO);
		glBindVertexArray(m_iQuadVAO);

		unsigned int VBO, EBO;
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(m_iQuadVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texture coord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(1);

		m_bSetupQuad = true;
	}
	glBindVertexArray(m_iQuadVAO);
}

void SpriteSystem::OnRender(Sprite* comp)
{
	if (comp->m_spriteTransform == nullptr)
	{
		comp->m_spriteTransform = Application::GetEntity(comp->entityID)->GetComponent<Transform>();

		glm::vec3 scale = comp->m_spriteTransform->m_scale;
		float heightRat = (float)comp->m_spriteTexture->m_iWidth / (float)comp->m_spriteTexture->m_iHeight;

		comp->m_spriteTransform->m_scale = glm::normalize(glm::vec3(comp->m_spriteTexture->m_iWidth * heightRat, comp->m_spriteTexture->m_iHeight * heightRat, scale.z));
		comp->m_spriteTransform->m_scale.x *= scale.x;
		comp->m_spriteTransform->m_scale.y *= scale.y;

		return;
	}
	if (comp->m_spriteTexture == nullptr)
		return;

	Application::m_mainShaderProgram->UseProgram();
	glm::mat4 finalMat = Renderer::GenerateOrthographicMatrix() * Renderer::GenerateViewMatrix(); 
	glBindTexture(GL_TEXTURE_2D, comp->m_spriteTexture->m_iTextureID);

	Application::m_mainShaderProgram->BindMat4("vpMat", finalMat);
	Application::m_mainShaderProgram->BindMat4("transMat", comp->m_spriteTransform->m_transformMat);
	Application::m_mainShaderProgram->BindVector4("colour", comp->m_colour);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}