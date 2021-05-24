#include "Component.h"
#include "System.h"

namespace NobleCore
{
	void ComponentBase::AddToEntity(unsigned int _ID)
	{
		componentSystem.lock()->AddComponent(_ID);
	}
}