#include <NobleEngine/Core/Application.h>
#include <NobleEngine/Components/Transform.hpp>

using namespace NobleCore;
using namespace NobleComponents;
int main()
{
	std::shared_ptr<Application> app = Application::InitializeEngine("Test Program", 500, 500);

	for (int i = 0; i < 100000; i++)
	{
		Entity* temp = Application::CreateEntity();
		temp->AddComponent<Transform>();
	}

	app->MainEngineLoop();

	return 0;
}