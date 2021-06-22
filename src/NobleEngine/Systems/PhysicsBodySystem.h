#include "../ECS/System.hpp"
#include "../Components/PhysicsBody.hpp"

namespace NobleCore
{
	/**
	*Responsible for updating physics body objects. Automatically bound to the engine on startup.
	*/
	struct PhysicsBodySystem : public System<NobleComponents::PhysicsBody>
	{
		void OnUpdate(NobleComponents::PhysicsBody* comp);
	};
}