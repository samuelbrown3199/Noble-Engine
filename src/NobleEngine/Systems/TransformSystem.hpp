#include "../ECS/System.h"
#include "../Components/Transform.hpp"

namespace NobleCore
{
	struct TransformSystem : public NobleCore::System<NobleComponents::Transform>
	{
		void OnUpdate(NobleComponents::Transform* comp)
		{
			std::cout << "Transform info: " << comp->position.x << "  " << comp->position.y << "  " << comp->position.z << std::endl;
		}
	};
	
	SetupComponent(NobleComponents::Transform, TransformSystem);
}