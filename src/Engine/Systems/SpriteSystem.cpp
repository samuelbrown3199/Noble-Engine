#include "SpriteSystem.h"
#include "../Core/Application.h"

#include <iostream>

std::weak_ptr<SystemBase> SpriteSystem::self;
std::weak_ptr<SystemBase> Sprite::componentSystem;
std::vector<Sprite> Sprite::componentData;

void SpriteSystem::PreRender()
{

}

void SpriteSystem::OnRender(Sprite* comp)
{
	if (comp->m_spriteTransform == nullptr || Application::GetEntitiesDeleted())
	{
		comp->m_spriteTransform = Transform::GetComponent(comp->m_sEntityID);
		return;
	}
	if (comp->m_spriteTexture == nullptr)
		return;

}