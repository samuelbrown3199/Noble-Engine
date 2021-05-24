
#include "System.h"
#include "Entity.h"

int main()
{
	Application app;
	app.InitializeApplication();
	
	std::shared_ptr<TransformSystem> ts = app.BindSystem<TransformSystem>();

	int amount = 10000;
	Entity* temp;
	for (int i = 0; i < amount; i++)
	{
		temp = app.CreateEntity();
		temp->AddComponent<Transform>();
	}
	std::cout << "Created " << amount << " components." << std::endl;

	Transform* transform = temp->GetComponent<Transform>();
	if (transform != nullptr)
	{
		std::cout << "Entity contains transform!" << std::endl;
	}
	else
	{
		std::cout << "Entity does not contain transform!" << std::endl;
	}

	app.MainLoop();
	return 0;
}