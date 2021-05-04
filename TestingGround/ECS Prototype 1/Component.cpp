#include "Component.h"

#include "System.h"

void Component::AddToEntity(int ID)
{
	componentSystem.lock()->AddComponent(ID);
}