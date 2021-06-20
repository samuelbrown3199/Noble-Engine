#include "../ECS/System.hpp"
#include "../Components/Mesh.hpp"
#include "../Components/Transform.hpp"

namespace NobleCore
{
	struct MeshSystem : public System<NobleComponents::Mesh>
	{
		void OnRender(NobleComponents::Mesh* comp);
	};
}