#include "TransformSystem.h"

namespace NobleCore
{
	SetupComponent(NobleComponents::Transform, TransformSystem);

	void TransformSystem::OnUpdate(NobleComponents::Transform* comp)
	{
		if (comp->position != comp->oldPosition || comp->rotation != comp->oldRotation || comp->scale != comp->oldScale)
		{
			comp->modelMat = glm::mat4(1.0f);

			comp->modelMat = glm::translate(comp->modelMat, comp->position);
			comp->modelMat = glm::rotate(comp->modelMat, glm::radians(comp->rotation.x), glm::vec3(1, 0, 0));
			comp->modelMat = glm::rotate(comp->modelMat, glm::radians(comp->rotation.y), glm::vec3(0, 1, 0));
			comp->modelMat = glm::rotate(comp->modelMat, glm::radians(comp->rotation.z), glm::vec3(0, 0, 1));
			comp->modelMat = glm::scale(comp->modelMat, comp->scale);

			comp->oldPosition = comp->position;
			comp->oldRotation = comp->rotation;
			comp->oldScale = comp->scale;
		}
	};
}