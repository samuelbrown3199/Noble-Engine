#include "MeshRenderer.h"

#include "../Core/Application.h"
#include "../Core/Graphics/Renderer.h"
#include "../ECS/Entity.hpp"

ComponentDatalist<MeshRenderer> MeshRenderer::m_componentList;

void MeshRenderer::AddComponent()
{
	m_componentList.AddComponent(this);
}

void MeshRenderer::AddComponentToEntity(std::string entityID)
{
	m_componentList.AddComponentToEntity(entityID);
	Application::GetEntity(entityID)->GetAllComponents();
}

void MeshRenderer::RemoveComponent(std::string entityID)
{
	m_componentList.RemoveComponent(entityID);
}

void MeshRenderer::RemoveAllComponents()
{
	m_componentList.RemoveAllComponents();
}

MeshRenderer* MeshRenderer::GetComponent(std::string entityID)
{
	return m_componentList.GetComponent(entityID);
}

void MeshRenderer::Update(bool useThreads, int maxComponentsPerThread) {}

void MeshRenderer::Render(bool useThreads, int maxComponentsPerThread)
{
	m_componentList.Render(useThreads, maxComponentsPerThread);
}

void MeshRenderer::OnRender() 
{
	if (m_transform == nullptr)
	{
		m_transform = Application::GetEntity(m_sEntityID)->GetComponent<Transform>();
		return;
	}

	if (m_model == nullptr)
		return;

	if (m_shader == nullptr)
		return;

	m_bOnScreen = false;
	for (int i = 0; i < m_model->m_modelBoundingBox.size(); i++)
	{
		glm::vec3 transPos = m_transform->m_transformMat * glm::vec4(m_model->m_modelBoundingBox.at(i), 1.0f);
		if (IsPointInViewFrustum(transPos, Renderer::GenerateProjMatrix() * Renderer::GenerateViewMatrix()))
		{
			m_bOnScreen = true;
			break;
		}
	}
	if (!m_bOnScreen)
	{
		Logger::LogInformation("Not rendering mesh");
		return;
	}

	m_shader->UseProgram();

	glBindTexture(GL_TEXTURE_2D, m_texture->m_iTextureID);
	glBindVertexArray(m_model->m_vaoID);

	m_shader->BindMat4("transMat", m_transform->m_transformMat);
	m_shader->BindVector4("colour", m_colour);

	glDrawElements(Renderer::GetRenderMode(), m_model->m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void MeshRenderer::LoadComponentDataFromJson(nlohmann::json& j)
{
	m_componentList.LoadComponentDataFromJson(j);
}

nlohmann::json MeshRenderer::WriteComponentDataToJson()
{
	return m_componentList.WriteComponentDataToJson();
}