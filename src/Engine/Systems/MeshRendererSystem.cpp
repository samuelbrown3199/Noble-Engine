#include "MeshRendererSystem.h"
#include "../Core/Application.h"
#include "../Core/Renderer.h"

#include <iostream>

std::weak_ptr<SystemBase> MeshRendererSystem::self;
std::weak_ptr<SystemBase> MeshRenderer::componentSystem;
std::vector<MeshRenderer> MeshRenderer::componentData;

void MeshRendererSystem::OnRender(MeshRenderer* comp)
{
	if (comp->m_transform == nullptr)
	{
		comp->m_transform = Application::GetEntity(comp->entityID)->GetComponent<Transform>();
		return;
	}
	if (comp->m_texture == nullptr)
		return;

	Application::m_mainShaderProgram->UseProgram();
	glm::mat4 finalMat = Renderer::GenerateProjMatrix() * Renderer::GenerateViewMatrix();
	glBindTexture(GL_TEXTURE_2D, comp->m_texture->m_iTextureID);
	glBindVertexArray(comp->m_model->m_vaoID);

	Application::m_mainShaderProgram->BindMat4("vpMat", finalMat);
	Application::m_mainShaderProgram->BindMat4("transMat", comp->m_transform->m_transformMat);
	Application::m_mainShaderProgram->BindVector4("colour", comp->m_colour);

	glDrawArrays(Renderer::GetRenderMode(), 0, comp->m_model->m_drawCount);
	glBindVertexArray(0);
}