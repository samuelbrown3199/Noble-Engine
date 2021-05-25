#include "../ECS/System.h"
#include "Transform.hpp"

namespace NobleCore
{
	struct TransformSystem : public System<Transform>
	{
		void OnUpdate(Transform* comp)
		{
			std::cout << "Transform info: " << comp->position.x << "  " << comp->position.y << "  " << comp->position.z << std::endl;
		}
	};

	SetupComponent(Transform, TransformSystem);
}