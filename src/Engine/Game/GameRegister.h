#pragma once

//Used to register game resources, components and behaviours. Keeps it seperate from engine code.
struct GameRegister
{
private:

	void RegisterComponents();
	void RegisterResources();
	void RegisterBehaviours();

public:

	void RegisterGame();
};