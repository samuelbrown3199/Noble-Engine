#include <NobleEngine/Core/Application.h>
#include <NobleEngine/Components/Transform.hpp>
#include <NobleEngine/Components/StaticTransform.hpp>

using namespace NobleCore;
using namespace NobleComponents;
int main()
{
	std::shared_ptr<Application> app = Application::InitializeEngine("Test Program", GraphicsAPI::OpenGL, 500, 500);

	for (int i = 0; i < 10; i++)
	{
		Entity* temp = Application::CreateEntity();
		temp->AddComponent<StaticTransform>();
	}
	for (int i = 0; i < 100; i++)
	{
		Entity* temp = Application::CreateEntity();
		temp->AddComponent<Transform>();
	}

	app->MainEngineLoop();

	return 0;
}