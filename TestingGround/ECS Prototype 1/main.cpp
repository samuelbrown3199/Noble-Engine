
#include "System.h"

int main()
{
	Application app;
	app.InitializeApplication();
	app.CreateEntity();
	
	std::shared_ptr<TransformSystem> ts = app.BindSystem<TransformSystem>();
	ts->AddComponent(app.entities.at(0));

	app.MainLoop();
	return 0;
}