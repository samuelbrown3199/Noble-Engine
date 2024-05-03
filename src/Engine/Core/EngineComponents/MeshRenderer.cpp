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

	static ResourceSelectionWidget modelWidget;
	modelWidget.m_pComponent = this;
	modelWidget.m_sResourceType = "Model";
	//modelWidget.DoResourceSelection("Model", m_bInitializeInterface, m_model); //doesnt work yet, needs more work

	ChangeModel(std::dynamic_pointer_cast<Model>(rManager->DoResourceSelectInterface("Model", m_model != nullptr ? m_model->m_sLocalPath : "none", "Model")));)
	ChangeTexture(std::dynamic_pointer_cast<Texture>(rManager->DoResourceSelectInterface("Texture", m_texture != nullptr ? m_texture->m_sLocalPath : "none", "Texture")));
	ChangePipeline(std::dynamic_pointer_cast<Pipeline>(rManager->DoResourceSelectInterface("Pipeline", m_pipeline != nullptr ? m_pipeline->m_sLocalPath : "none", "Pipeline")));

	static NobleColourEdit colourEdit;
	colourEdit.m_pComponent = this;
	colourEdit.DoColourEdit4("Colour", m_bInitializeInterface, &m_colour);

	m_bInitializeInterface = false;
}