
#include "System.h"

int main()
{
	Application app;
	app.InitializeApplication();
	
	std::shared_ptr<TransformSystem> ts = app.BindSystem<TransformSystem>();

	for (int i = 0; i < 100; i++)
	{
		app.CreateEntity();
		Transform::AddToComponent(app.entities.at(i));
	}

	app.MainLoop();
	return 0;
}