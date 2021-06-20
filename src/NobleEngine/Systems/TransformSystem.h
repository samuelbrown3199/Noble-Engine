#include "../ECS/System.hpp"
#include "../Components/Transform.hpp"

#include <glm/ext.hpp>

namespace NobleCore
{
	struct TransformSystem : public NobleCore::System<NobleComponents::Transform>
	{
		void OnUpdate(NobleComponents::Transform* comp);
	};
}