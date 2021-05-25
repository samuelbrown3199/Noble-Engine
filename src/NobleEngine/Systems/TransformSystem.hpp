#include "../ECS/System.hpp"
#include "../Components/Transform.hpp"

#include <glm/ext.hpp>

namespace NobleCore
{
	struct TransformSystem : public NobleCore::System<NobleComponents::Transform>
	{
		void OnUpdate(NobleComponents::Transform* comp)
		{
			comp->modelMat = glm::mat4(1.0f);

			comp->modelMat = glm::translate(comp->modelMat, comp->position);
			comp->modelMat = glm::rotate(comp->modelMat, glm::radians(comp->rotation.x), glm::vec3(1, 0, 0));
			comp->modelMat = glm::rotate(comp->modelMat, glm::radians(comp->rotation.y), glm::vec3(0, 1, 0));
			comp->modelMat = glm::rotate(comp->modelMat, glm::radians(comp->rotation.z), glm::vec3(0, 0, 1));
			comp->modelMat = glm::scale(comp->modelMat, comp->scale);
		}
	};
	
	SetupComponent(NobleComponents::Transform, TransformSystem);
}