#include "Application.h"
#include "InputManager.h"

#include <iostream>

namespace NobleCore
{
	bool Application::loop = true;
	std::weak_ptr<Application> Application::self;
	std::shared_ptr<Screen> Application::screen;

	std::shared_ptr<Application> Application::InitializeEngine()
	{
		std::shared_ptr<Application> app = std::make_shared<Application>();
		Application::self = app;

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == -1)
		{
			std::cout << "Failed to initialize SDL!" << std::endl;
			throw std::exception();
		}
		Application::screen = std::make_shared<Screen>("temp", 500, 500);

		return app;
	}

	void Application::MainEngineLoop()
	{
		while (loop)
		{
			//frame start
			InputManager::HandleGeneralInput();

			//update

			//render

			//frame cleanup
			InputManager::ClearFrameInputs();
		}
	}
}