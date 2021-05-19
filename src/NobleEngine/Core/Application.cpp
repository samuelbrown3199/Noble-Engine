#include "Application.h"
#include "InputManager.h"

namespace NobleCore
{
	bool Application::loop = true;
	std::weak_ptr<Application> Application::self;
	std::shared_ptr<Screen> Application::screen;

	std::shared_ptr<Application> Application::InitializeEngine(std::string _windowName, int _windowWidth, int _windowHeight)
	{
		std::shared_ptr<Application> app = std::make_shared<Application>();
		Application::self = app;

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == -1)
		{
			std::cout << "Failed to initialize SDL!" << std::endl;
			throw std::exception();
		}
		Application::screen = std::make_shared<Screen>(_windowName, _windowWidth, _windowHeight);

		return app;
	}

	void Application::MainEngineLoop()
	{
		while (loop)
		{
			//frame start
			InputManager::HandleGeneralInput();
			Screen::UpdateScreenSize();

			//update

			//render

			//frame cleanup
			InputManager::ClearFrameInputs();
		}
	}
}