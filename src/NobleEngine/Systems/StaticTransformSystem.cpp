#include "StaticTransformSystem.h"

#include <glm/gtx/string_cast.hpp>

namespace NobleCore
{
	SetupComponent(NobleComponents::StaticTransform, StaticTransformSystem);

	void StaticTransformSystem::OnUpdate(NobleComponents::StaticTransform* comp)
	{
		if (comp->dirty)
		{
			comp->model = glm::mat4(1.0f);

			comp->model = glm::translate(comp->model, comp->position);
			comp->model = glm::rotate(comp->model, glm::radians(comp->rotation.x), glm::vec3(1, 0, 0));
			comp->model = glm::rotate(comp->model, glm::radians(comp->rotation.y), glm::vec3(0, 1, 0));
			comp->model = glm::rotate(comp->model, glm::radians(comp->rotation.z), glm::vec3(0, 0, 1));
			comp->model = glm::scale(comp->model, comp->scale);
			comp->dirty = false;
		}
	}
}