#include "Component.h"

#include "System.h"

void ComponentBase::AddToEntity(int ID)
{
	componentSystem.lock()->AddComponent(ID);
}