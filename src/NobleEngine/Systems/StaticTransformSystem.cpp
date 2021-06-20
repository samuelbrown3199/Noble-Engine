#include "StaticTransformSystem.h"

namespace NobleCore
{
	SetupComponent(NobleComponents::StaticTransform, StaticTransformSystem);

	void StaticTransformSystem::OnUpdate(NobleComponents::StaticTransform* comp)
	{
		if (!comp->dirty)
		{
			comp->modelMat = glm::translate(comp->modelMat, comp->position);
			comp->modelMat = glm::rotate(comp->modelMat, glm::radians(comp->rotation.x), glm::vec3(1, 0, 0));
			comp->modelMat = glm::rotate(comp->modelMat, glm::radians(comp->rotation.y), glm::vec3(0, 1, 0));
			comp->modelMat = glm::rotate(comp->modelMat, glm::radians(comp->rotation.z), glm::vec3(0, 0, 1));
			comp->modelMat = glm::scale(comp->modelMat, comp->scale);
			comp->dirty = true;
		}
	}
}