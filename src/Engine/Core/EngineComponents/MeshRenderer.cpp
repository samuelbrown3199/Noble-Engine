#include "MeshRenderer.h"

#include "../Application.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/BufferHelper.h"
#include "../Graphics/GraphicsPipeline.h"
#include "../ECS/Entity.hpp"

void MeshRenderer::OnPreRender() 
{
	if (m_model == nullptr)
		return;

	m_vertices = &m_model->m_vertices;
	m_indices = &m_model->m_indices;
	m_boundingBox = &m_model->m_modelBoundingBox;

	m_meshBuffers = m_model->m_meshBuffer;

	Renderable::OnPreRender();

	Transform* transform = NobleRegistry::GetComponent<Transform>(m_transformIndex);
	if (transform == nullptr)
		return;

	if (!m_bOnScreen)
		return;

	m_drawConstants.m_worldMatrix = Renderer::GenerateProjMatrix() * Renderer::GenerateViewMatrix() * transform->m_transformMat;
}