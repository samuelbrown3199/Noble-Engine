#include "../ECS/System.hpp"
#include "../Components/Transform.hpp"

#include <glm/ext.hpp>

namespace NobleCore
{
	struct TransformSystem : public NobleCore::System<NobleComponents::Transform>
	{
		glm::mat4 GenerateModelMatrix(NobleComponents::Transform* comp)
		{
			glm::mat4 tempMat = glm::mat4(1.0f);

			tempMat = glm::translate(comp->modelMat, comp->position);
			tempMat = glm::rotate(comp->modelMat, glm::radians(comp->rotation.x), glm::vec3(1, 0, 0));
			tempMat = glm::rotate(comp->modelMat, glm::radians(comp->rotation.y), glm::vec3(0, 1, 0));
			tempMat = glm::rotate(comp->modelMat, glm::radians(comp->rotation.z), glm::vec3(0, 0, 1));
			tempMat = glm::scale(comp->modelMat, comp->scale);

			return tempMat;
			comp->modelMat = comp->modelMat != tempMat ? comp->modelMat : tempMat;
		}

		void OnUpdate(NobleComponents::Transform* comp)
		{
			//needs work to be more optimal. Might be worth adding a static transform component for separated functionality.
			comp->modelMat = GenerateModelMatrix(comp);
		}
	};
	
	SetupComponent(NobleComponents::Transform, TransformSystem);
}