#include "Application.h"
#include "InputManager.h"

#include "../Systems/StaticTransformSystem.hpp"
#include "../Systems/TransformSystem.hpp"

namespace NobleCore
{
	bool Application::loop = true;
	std::weak_ptr<Application> Application::self;
	std::shared_ptr<Screen> Application::screen; 
	std::shared_ptr<Renderer> Application::renderer;
	std::shared_ptr<AudioManager> Application::audioManager;

	std::vector<Entity> Application::entities;
	std::vector<std::shared_ptr<SystemBase>> Application::componentSystems;

	void Application::BindCoreSystems()
	{
		BindSystem<StaticTransformSystem>(true, false);
		BindSystem<TransformSystem>(true, false);
	}

	std::shared_ptr<Application> Application::InitializeEngine(std::string _windowName, GraphicsAPI _graphicsAPI, int _windowWidth, int _windowHeight)
	{
		std::shared_ptr<Application> app = std::make_shared<Application>();
		Application::self = app;

		if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
		{
			std::cout << "Failed to initialize SDL!" << std::endl;
			throw std::exception();
		}
		Application::screen = std::make_shared<Screen>(_windowName, _windowWidth, _windowHeight);
		Application::renderer = std::make_shared<Renderer>(_graphicsAPI);
		Application::audioManager = std::make_shared<AudioManager>();
		Application::BindCoreSystems();

		return app;
	}

	void Application::MainEngineLoop()
	{
		while (loop)
		{
			float frameStart = SDL_GetTicks();

			//frame start
			InputManager::HandleGeneralInput();
			Screen::UpdateScreenSize();

			//update
			for (int i = 0; i < componentSystems.size(); i++)
			{
				componentSystems.at(i)->Update();
			}
			//render
			for (int i = 0; i < componentSystems.size(); i++)
			{
				componentSystems.at(i)->Render();
			}
			//frame cleanup
			InputManager::ClearFrameInputs();

			float frameEnd = SDL_GetTicks() - frameStart;
			std::cout << "Frame Time " << frameEnd << std::endl;
		}
	}

	Entity* Application::CreateEntity()
	{
		entities.push_back(entities.size());
		return &entities.at(entities.size() - 1);
	}

	Entity* Application::GetEntity(unsigned int _ID)
	{
		for (int i = 0; i < entities.size(); i++)
		{
			if (entities.at(i).entityID == _ID)
			{
				return &entities.at(i);
			}
		}

		return nullptr;
	}
}