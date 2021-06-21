#include "TransformSystem.h"

#include "glm/gtx/string_cast.hpp"

namespace NobleCore
{
	SetupComponent(NobleComponents::Transform, TransformSystem);

	void TransformSystem::OnUpdate(NobleComponents::Transform* comp)
	{
		comp->rotation.x += 0.75f;
		comp->rotation.y += 1.0f;
		comp->rotation.z += 0.33f;

		if (comp->position != comp->oldPosition || comp->rotation != comp->oldRotation || comp->scale != comp->oldScale)
		{
			comp->model = glm::mat4(1.0f);

			comp->model = glm::translate(comp->model, comp->position);
			comp->model = glm::rotate(comp->model, glm::radians(comp->rotation.x), glm::vec3(1, 0, 0));
			comp->model = glm::rotate(comp->model, glm::radians(comp->rotation.y), glm::vec3(0, 1, 0));
			comp->model = glm::rotate(comp->model, glm::radians(comp->rotation.z), glm::vec3(0, 0, 1));
			comp->model = glm::scale(comp->model, comp->scale);

			comp->oldPosition = comp->position;
			comp->oldRotation = comp->rotation;
			comp->oldScale = comp->scale;
		}
	}
}