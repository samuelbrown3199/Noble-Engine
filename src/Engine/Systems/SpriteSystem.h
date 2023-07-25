#include "../ECS/System.hpp"
#include "Sprite.h"

#include "../Core/Graphics/BufferHelper.h"

struct SpriteSystem : public System<Sprite>
{
	bool m_bInitializedSpriteQuad = false;

	GraphicsBuffer m_vertexBuffer;
	GraphicsBuffer m_indexBuffer;

	void PreRender();
	void OnRender(Sprite* comp);
};