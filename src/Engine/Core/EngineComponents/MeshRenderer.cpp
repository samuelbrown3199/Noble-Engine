#include "MeshRenderer.h"

#include "../Application.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/BufferHelper.h"
#include "../Graphics/GraphicsPipeline.h"
#include "../ECS/Entity.h"

void MeshRenderer::OnPreRender() 
{
	if (m_model == nullptr)
		return;

	m_vertices = &m_model->m_vertices;
	m_indices = &m_model->m_indices;
	m_boundingBox = &m_model->m_modelBoundingBox;

	m_meshBuffers = m_model->m_meshBuffer;

	Renderable::OnPreRender();
}

void MeshRenderer::DoComponentInterface()
{
	ResourceManager* rManager = Application::GetApplication()->GetResourceManager();

	if (m_transformIndex == -1)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		ImGui::Text("No transform attached. Object won't render.");
		ImGui::PopStyleColor();

		ImGui::Dummy(ImVec2(0.0f, 5.0f));
	}

	ImGui::BeginDisabled();
	ImGui::Checkbox("On Screen", &m_bOnScreen);
	ImGui::EndDisabled();

	ChangeModel(rManager->DoResourceSelectInterface<Model>("Model", m_model != nullptr ? m_model->m_sLocalPath : "none"));
	ChangeTexture(rManager->DoResourceSelectInterface<Texture>("Texture", m_texture != nullptr ? m_texture->m_sLocalPath : "none"));
	ChangePipeline(rManager->DoResourceSelectInterface<Pipeline>("Pipeline", m_pipeline != nullptr ? m_pipeline->m_sLocalPath : "none"));

	static NobleColourEdit colourEdit;
	colourEdit.m_pComponent = this;
	colourEdit.DoColourEdit4("Colour", m_bInitializeInterface, &m_colour);

	m_bInitializeInterface = false;
}