#include "Component.h"

#include "System.h"

void Component::AddToComponent(int ID)
{
	componentSystem.lock()->AddComponent(ID);
}