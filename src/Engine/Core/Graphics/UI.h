#pragma once

#include <iostream>
#include <string>

#include <GL/glew.h>
#include<glm/glm.hpp>

#include "../ResourceManager.h"
#include "../InputManager.h"
#include "Renderer.h"

#include "../../Resource/ShaderProgram.h"
#include "../../Resource/Texture.h"
#include "../../Resource/Font.h"

class UIQuads
{
	static unsigned int m_iQuadVAO, m_iTextVAO, m_iTextVBO;

	static void SetupQuad()
	{
		const GLfloat positions[] =
		{
			0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f
		};

		const GLfloat uvs[] =
		{
			0.0, 0.0,
			0.0, 1.0,
			1.0, 1.0,
			1.0, 1.0,
			1.0, 0.0,
			0.0, 0.0
		};

		GLuint positionsVBO = 0;
		glGenBuffers(1, &positionsVBO);
		if (!positionsVBO)
		{
			throw std::exception();
		}
		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint uvVBO = 0;
		glGenBuffers(1, &uvVBO);
		if (!uvVBO)
		{
			throw std::exception();
		}
		glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &m_iQuadVAO);
		if (!m_iQuadVAO)
		{
			throw std::exception();
		}
		glBindVertexArray(m_iQuadVAO);

		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}

	static void SetupTextQuad()
	{
		glGenVertexArrays(1, &m_iTextVAO);
		glGenBuffers(1, &m_iTextVBO);
		glBindVertexArray(m_iTextVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_iTextVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

public:
	static void SetupUIQuads()
	{
		SetupQuad();
		SetupTextQuad();

		Logger::LogInformation("Setup UI quads.");
	}
};