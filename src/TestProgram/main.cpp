#include <NobleEngine/Core/Application.h>
#include <NobleEngine/Components/Transform.hpp>
#include <NobleEngine/Components/StaticTransform.hpp>

using namespace NobleCore;
using namespace NobleComponents;
int main()
{
	std::shared_ptr<Application> app = Application::InitializeEngine("Test Program", GraphicsAPI::OpenGL, 500, 500);

	Entity* test = Application::CreateEntity();
	test->AddComponent<Transform>();
	test->RemoveComponent<Transform>();

	app->MainEngineLoop();

	return 0;
}