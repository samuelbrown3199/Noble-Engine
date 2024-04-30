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

	ChangeTexture(rManager->DoResourceSelectInterface<Texture>("Sprite", m_texture != nullptr ? m_texture->m_sLocalPath : "none"));
	ChangePipeline(rManager->DoResourceSelectInterface<Pipeline>("Pipeline", m_pipeline != nullptr ? m_pipeline->m_sLocalPath : "none"));

	static NobleColourEdit colourEdit;
	colourEdit.m_pComponent = this;
	colourEdit.DoColourEdit4("Colour", m_bInitializeInterface, &m_colour);

	m_bInitializeInterface = false;
}