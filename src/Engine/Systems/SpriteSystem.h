#include "../ECS/System.hpp"
#include "Sprite.h"

struct SpriteSystem : public System<Sprite>
{
	void PreRender();
	void OnRender(Sprite* comp);
};