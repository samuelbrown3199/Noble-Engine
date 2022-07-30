#include "../ECS/System.hpp"
#include "Transform.h"

struct TransformSystem : public System<Transform>
{
	void OnUpdate(Transform* comp);
};