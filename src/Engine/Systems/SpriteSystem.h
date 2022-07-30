#include "../ECS/System.hpp"
#include "Sprite.h"

#include <GL/glew.h>

struct SpriteSystem : public System<Sprite>
{
	static unsigned int m_iQuadVAO;
	bool m_bSetupQuad = false;

	void PreRender();
	void OnRender(Sprite* comp);
};