#include "Sprite.h"
#include "../Application.h"
#include "../Graphics/Renderer.h"
#include "../ECS/Entity.h"

std::vector<uint32_t> Sprite::spriteQuadIndices =
{
	0, 1, 2, 2, 3, 0
};
std::vector<Vertex> Sprite::spriteQuadVertices =
{
	Vertex(glm::vec3{ 0.0f, 0.0f, 0.0 }, glm::vec2{ 0.0f, 0.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }),
	Vertex(glm::vec3{ 1.0f, 0.0f, 0.0 }, glm::vec2{ 1.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }),
	Vertex(glm::vec3{ 1.0f, 1.0f, 0.0 }, glm::vec2{ 1.0f, 1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }),
	Vertex(glm::vec3{ 0.0f, 1.0f, 0.0 }, glm::vec2{ 0.0f, 1.0f }, glm::vec3{ 1.0f, 1.0f, 1.0f })
};
std::vector<glm::vec3> Sprite::boundingBox =
{
	glm::vec3(0.0f, 0.0f, 0.0),
	glm::vec3(1.0f, 0.0f, 0.0),
	glm::vec3(1.0f, 1.0f, 0.0),
	glm::vec3(0.0f, 1.0f, 0.0),
};

void Sprite::OnPreRender()
{
	Renderer* renderer = Application::GetApplication()->GetRenderer();

	m_meshBuffers = renderer->GetSpriteQuadBuffer();
	m_vertices = &spriteQuadVertices;
	m_boundingBox = &boundingBox;
	m_indices = &spriteQuadIndices;

	Renderable::OnPreRender();
}

void Sprite::DoComponentInterface()
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

	static ResourceSelectionWidget textureWidget;
	textureWidget.m_pComponent = this;
	textureWidget.m_sResourceType = "Texture";
	textureWidget.DoResourceSelection("Sprite", m_bInitializeInterface, &m_sTargetTexturePath);

	static ResourceSelectionWidget pipelineWidget;
	pipelineWidget.m_pComponent = this;
	pipelineWidget.m_sResourceType = "Pipeline";
	pipelineWidget.DoResourceSelection("Pipeline", m_bInitializeInterface, &m_sTargetPipelinePath);

	static NobleColourEdit colourEdit;
	colourEdit.m_pComponent = this;
	colourEdit.DoColourEdit4("Sprite Colour", m_bInitializeInterface, &m_colour);

	m_bInitializeInterface = false;
}