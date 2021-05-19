#include <NobleEngine/Core/Application.h>
using namespace NobleCore;

int main()
{
	std::shared_ptr<Application> app = Application::InitializeEngine();
	app->MainEngineLoop();

	return 0;
}