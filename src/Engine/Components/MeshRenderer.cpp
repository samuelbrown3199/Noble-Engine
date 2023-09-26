#include "MeshRenderer.h"

#include "../Core/Application.h"
#include "../Core/Graphics/Renderer.h"
#include "../ECS/Entity.hpp"

void MeshRenderer::OnRender() 
{
	if (m_model == nullptr)
		return;

	m_vertices = &m_model->m_vertices;
	m_indices = &m_model->m_indices;
	m_boundingBox = &m_model->m_modelBoundingBox;

	Renderable::OnRender();

	Transform* transform = NobleRegistry::GetComponent<Transform>(m_transformIndex);
	if (transform == nullptr)
		return;

	if (m_shader == nullptr)
		return;

	if (!m_bOnScreen)
		return;

	m_shader->UseProgram();

	glBindTexture(GL_TEXTURE_2D, m_texture->m_iTextureID);
	glBindVertexArray(m_model->m_vaoID);

	m_shader->BindMat4("transMat", transform->m_transformMat);
	m_shader->BindVector4("colour", m_colour);

	glDrawElements(Renderer::GetRenderMode(), m_model->m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}