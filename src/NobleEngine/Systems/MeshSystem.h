#include "../ECS/System.hpp"
#include "../Components/Mesh.hpp"
#include "../Components/Transform.hpp"
#include "../Components/StaticTransform.hpp"

namespace NobleCore
{
	struct MeshSystem : public System<NobleComponents::Mesh>
	{
		glm::mat4 projectionViewMatrix;

		void GetTransformInformation(NobleComponents::Mesh* comp);
		void PreRender();
		void OnRender(NobleComponents::Mesh* comp);
	};
}