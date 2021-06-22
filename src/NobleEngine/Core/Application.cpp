#include "Application.h"
#include "InputManager.h"

#include "ResourceManager.h"
#include "../Systems/StaticTransformSystem.h"
#include "../Systems/TransformSystem.h"
#include "../Systems/PhysicsBodySystem.h"
#include "../Systems/CameraSystem.h"
#include "../Systems/MeshSystem.h"

namespace NobleCore
{
	bool Application::loop = true;
	std::weak_ptr<Application> Application::self;
	PerformanceStats Application::performanceStats;
	std::shared_ptr<ThreadingManager> Application::threadingManager;
	std::shared_ptr<Screen> Application::screen; 
	std::shared_ptr<Renderer> Application::renderer;
	std::shared_ptr<AudioManager> Application::audioManager;
	std::shared_ptr<PhysicsWorld> Application::physicsWorld;
	std::vector<Entity*> Application::deletionEntities;

	std::vector<Entity> Application::entities;
	std::vector<std::shared_ptr<SystemBase>> Application::componentSystems;
	NobleComponents::Camera* Application::mainCam;

	void Application::BindCoreSystems()
	{
		BindSystem<StaticTransformSystem>(SystemUsage::useUpdate);
		BindSystem<TransformSystem>(SystemUsage::useUpdate, 20000);
		BindSystem<PhysicsBodySystem>(SystemUsage::useUpdate);
		BindSystem<CameraSystem>(SystemUsage::useUpdate);
		BindSystem<MeshSystem>(SystemUsage::useRender);
	}

	void Application::CleanupDeletionEntities()
	{
		for(int i = deletionEntities.size() - 1; i >= 0; i--)
		{
			for (int o = 0; o < componentSystems.size(); o++)
			{
				componentSystems.at(o)->RemoveComponent(deletionEntities.at(i)->entityID);
			}
			deletionEntities.at(i)->availableForUse = true;
			deletionEntities.pop_back();
		}
	}

	void Application::CleanupEngine()
	{
		physicsWorld->CleanupPhysicsWorld();
		ThreadingManager::StopThreads();
	}

	std::shared_ptr<Application> Application::InitializeEngine(std::string _windowName, GraphicsAPI _graphicsAPI, int _windowWidth, int _windowHeight)
	{
		std::shared_ptr<Application> app = std::make_shared<Application>();
		Application::self = app;
		Application::threadingManager = std::make_shared<ThreadingManager>();

		if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
		{
			std::cout << "Failed to initialize SDL!" << std::endl;
			throw std::exception();
		}
		Application::screen = std::make_shared<Screen>(_windowName, _windowWidth, _windowHeight);
		Application::renderer = std::make_shared<Renderer>(_graphicsAPI);
		Application::audioManager = std::make_shared<AudioManager>();
		Application::physicsWorld = PhysicsWorld::CreatePhysicsWorld();
		Application::BindCoreSystems();

		return app;
	}

	void Application::MainEngineLoop()
	{
		while (loop)
		{
			performanceStats.ResetPerformanceStats();
			performanceStats.preUpdateStart = SDL_GetTicks();

			//frame start
			InputManager::HandleGeneralInput();
			Screen::UpdateScreenSize();
			performanceStats.preUpdateTime = SDL_GetTicks() - performanceStats.preUpdateStart;

			performanceStats.updateStart = SDL_GetTicks();
			ThreadingManager::EnqueueTask([&] {physicsWorld->StepSimulation(PerformanceStats::deltaT); });
			//update
			for (int i = 0; i < componentSystems.size(); i++)
			{
				componentSystems.at(i)->PreUpdate();
				componentSystems.at(i)->Update();
			}
			ThreadingManager::WaitForTasksToClear();
			performanceStats.updateTime = SDL_GetTicks() - performanceStats.updateStart;

			//render
			performanceStats.renderStart = SDL_GetTicks();
			Renderer::ClearBuffer();
			for (int i = 0; i < componentSystems.size(); i++)
			{
				componentSystems.at(i)->PreRender();
				componentSystems.at(i)->Render();
			}
			ThreadingManager::WaitForTasksToClear();
			Renderer::SwapGraphicsBuffer();
			performanceStats.renderTime = SDL_GetTicks() - performanceStats.renderStart;

			//frame cleanup
			performanceStats.cleanupStart = SDL_GetTicks();
			InputManager::ClearFrameInputs();
			ResourceManager::UnloadUnusedResources();
			CleanupDeletionEntities();
			performanceStats.cleanupTime = SDL_GetTicks() - performanceStats.cleanupStart;

			performanceStats.UpdatePerformanceStats();
			performanceStats.PrintOutPerformanceStats();
		}

		//Program cleanup before exit
		CleanupEngine();
	}

	Entity* Application::CreateEntity() //this will need optimisation
	{
		for (int i = 0; i < entities.size(); i++)
		{
			if (entities.at(i).availableForUse)
			{
				entities.at(i).availableForUse = false;
				return &entities.at(i);
			}
		}
		entities.push_back(entities.size());
		return &entities.at(entities.size() - 1);
	}

	void Application::DeleteEntity(unsigned int _ID)
	{
		if (_ID <= entities.size())
		{
			deletionEntities.push_back(&entities.at(_ID));
		}
	}

	Entity* Application::GetEntity(unsigned int _ID)
	{
		if (_ID <= entities.size() - 1)
		{
			return &entities.at(_ID);
		}
		return nullptr;
	}

	void Application::SetMainCamera(NobleComponents::Camera* _mainCam)
	{
		mainCam = _mainCam;
	}
}