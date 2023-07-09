#include "MeshRendererSystem.h"
#include "../Core/Application.h"
#include "../Core/Renderer.h"

#include <iostream>

std::weak_ptr<SystemBase> MeshRendererSystem::self;
std::weak_ptr<SystemBase> MeshRenderer::componentSystem;
std::vector<MeshRenderer> MeshRenderer::componentData;

void MeshRendererSystem::OnRender(MeshRenderer* comp)
{
	//Command to draw our triangle. Parameters as follows, CommandBuffer, VertexCount, InstanceCount, firstVertex, firstInstance
	vkCmdDraw(Renderer::GetCurrentCommandBuffer(), 3, 1, 0, 0);
}