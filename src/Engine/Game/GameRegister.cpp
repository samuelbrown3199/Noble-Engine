#include "GameRegister.h"

#include "../Core/Logger.h"
#include "../Core/Registry.h"

void GameRegister::RegisterComponents()
{
	//fill out for game
}

void GameRegister::RegisterResources()
{
	//fill out for game
}

void GameRegister::RegisterBehaviours()
{
	//fill out for game
}

void GameRegister::RegisterGame()
{
	RegisterComponents();
	RegisterResources();
	RegisterBehaviours();

	Logger::LogInformation("Registered Game specific information.");
}