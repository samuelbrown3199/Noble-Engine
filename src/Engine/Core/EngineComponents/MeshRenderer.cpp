#include "MeshRenderer.h"

#include "../Application.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/BufferHelper.h"
#include "../Graphics/GraphicsPipeline.h"
#include "../ECS/Entity.h"

void MeshRenderer::OnPreRender() 
{
	if(m_model == nullptr && m_sTargetModelPath != "")
		m_model = Application::GetApplication()->GetResourceManager()->LoadResource<Model>(m_sTargetModelPath);
	else if(!m_model->CheckIfLocalPathMatches(m_sTargetModelPath))
		m_model = Application::GetApplication()->GetResourceManager()->LoadResource<Model>(m_sTargetModelPath);

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
	modelWidget.DoResourceSelection("Model", m_bInitializeInterface, &m_sTargetModelPath);


	static ResourceSelectionWidget textureWidget;
	textureWidget.m_pComponent = this;
	textureWidget.m_sResourceType = "Texture";
	textureWidget.DoResourceSelection("Texture", m_bInitializeInterface, &m_sTargetTexturePath);

	static ResourceSelectionWidget pipelineWidget;
	pipelineWidget.m_pComponent = this;
	pipelineWidget.m_sResourceType = "Pipeline";
	pipelineWidget.DoResourceSelection("Pipeline", m_bInitializeInterface, &m_sTargetPipelinePath);

	static NobleColourEdit colourEdit;
	colourEdit.m_pComponent = this;
	colourEdit.DoColourEdit4("Colour", m_bInitializeInterface, &m_colour);

	m_bInitializeInterface = false;
}