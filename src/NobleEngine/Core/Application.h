#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "ThreadingManager.h"
#include "Screen.h"
#include "Renderer.h"
#include "AudioManager.h"
#include "../ECS/Entity.hpp"
#include "../ECS/System.hpp"
#include "../Components/Camera.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <exception>
#include <vector>

#include <SDL/SDL.h>

namespace NobleCore
{
	struct SystemBase;
	struct Entity;

	/**
	*Stores core information and handles the main loop.
	*/
	class Application
	{
		friend class InputManager;

	private:
		/**
		*Is the main loop running?
		*/
		static bool loop;
		/**
		*Stores a weak pointer to the application.
		*/
		static std::weak_ptr<Application> self;
		/**
		* Stores relevant information for threading.
		*/
		static std::shared_ptr<ThreadingManager> threadingManager;
		/**
		*Stores the application screen information.
		*/
		static std::shared_ptr<Screen> screen;
		/**
		* Stores the engines rendering information.
		*/
		static std::shared_ptr<Renderer> renderer;
		/**
		*Stores the applications audio systems.
		*/
		static std::shared_ptr<AudioManager> audioManager;
		/**
		*Stores the entities marked for deletion.
		*/
		static std::vector<Entity*> deletionEntities;
		/**
		*Stores the engines component systems.
		*/
		static std::vector<std::shared_ptr<SystemBase>> componentSystems;

		static void BindCoreSystems();
		static void CleanupDeletionEntities();
		static void CleanupEngine();
	public:

		/**
		*Stores the current entities from the loaded scene.
		*/
		static std::vector<Entity> entities;
		static NobleComponents::Camera* mainCam;
		/**
		*Initializes the engine.
		*/
		static std::shared_ptr<Application> InitializeEngine(std::string _windowName, GraphicsAPI _graphicsAPI, int _windowWidth, int _windowHeightt);
		/**
		*The main engine loop.
		*/
		static void MainEngineLoop();
		/**
		*Creates and returns an entity.
		*/
		static Entity* CreateEntity();
		/**
		* Finds the entity with the entityID of parameter _ID and clears the entity and component data.
		*/
		static void DeleteEntity(unsigned int _ID);
		/**
		*Finds and returns the entity with entityID of parameter _ID.
		*/
		static Entity* GetEntity(unsigned int _ID);
		/**
		*Creates and binds the system of type T.
		*/
		template<typename T>
		static std::shared_ptr<T> BindSystem(bool _useUpdate, bool _useRender)
		{
			std::shared_ptr<T> temp;
			for (size_t sys = 0; sys < componentSystems.size(); sys++)
			{
				temp = std::dynamic_pointer_cast<T>(componentSystems.at(sys));
				if (temp)
				{
					std::cout << "System is already bound!!" << std::endl;
					return temp;
				}
			}

			std::shared_ptr<T> system = std::make_shared<T>();
			system->self = system;
			system->useUpdate = _useUpdate;
			system->useRender = _useRender;
			system->useThreads = false;
			system->InitializeSystem();
			componentSystems.push_back(system);
			return system;
		}
		/**
		*Creates and binds the system of type T, but the system will be threaded with components being split up into groups.
		*/
		template<typename T>
		static std::shared_ptr<T> BindSystem(bool _useUpdate, bool _useRender, int _componentsPerThread)
		{
			std::shared_ptr<T> temp;
			for (size_t sys = 0; sys < componentSystems.size(); sys++)
			{
				temp = std::dynamic_pointer_cast<T>(componentSystems.at(sys));
				if (temp)
				{
					std::cout << "System is already bound!!" << std::endl;
					return temp;
				}
			}

			std::shared_ptr<T> system = std::make_shared<T>();
			system->self = system;
			system->useUpdate = _useUpdate;
			system->useRender = _useRender;
			system->useThreads = true;
			system->maxComponentsPerThread = _componentsPerThread;
			system->InitializeSystem();
			componentSystems.push_back(system);
			return system;
		}
		/**
		* Used to set the main camera for the application.
		*/
		static void SetMainCamera(NobleComponents::Camera* _mainCam);
	};
}

#endif