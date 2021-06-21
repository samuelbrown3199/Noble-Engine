#include "../ECS/System.hpp"
#include "../Components/Camera.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace NobleCore
{
	struct CameraSystem : public System<NobleComponents::Camera>
	{
		void OnUpdate(NobleComponents::Camera* comp);

		void GenerateProjectionMatrix(NobleComponents::Camera* comp);
		void GenerateViewMatrix(NobleComponents::Camera* comp);
	};
}