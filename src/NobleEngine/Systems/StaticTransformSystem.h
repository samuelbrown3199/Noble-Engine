#include "../ECS/System.hpp"
#include "../Components/StaticTransform.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace NobleCore
{
	struct StaticTransformSystem : public System<NobleComponents::StaticTransform>
	{
		void OnUpdate(NobleComponents::StaticTransform* comp);
	};
}