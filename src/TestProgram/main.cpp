#include <NobleEngine/Core/Application.h>
using namespace NobleCore;

int main()
{
	std::shared_ptr<Application> app = Application::InitializeEngine("Test Program", 500, 500);
	app->MainEngineLoop();

	return 0;
}