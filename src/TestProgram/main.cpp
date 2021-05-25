#include <NobleEngine/Core/Application.h>
#include <NobleEngine/Components/Transform.hpp>

using namespace NobleCore;
int main()
{
	std::shared_ptr<Application> app = Application::InitializeEngine("Test Program", 500, 500);

	Entity* test = Application::CreateEntity();
	test->AddComponent<Transform>(glm::vec3(500, 5, 20), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));

	app->MainEngineLoop();

	return 0;
}