#include "../ECS/System.hpp"
#include "../Components/StaticTransform.hpp"

#include <glm/ext.hpp>

namespace NobleCore
{
	struct StaticTransformSystem : public System<NobleComponents::StaticTransform>
	{

	};
}