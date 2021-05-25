#include "Screen.h"
#include "AudioManager.h"
#include "../ECS/Entity.h"
#include "../ECS/System.h"

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
		*Stores the application screen information.
		*/
		static std::shared_ptr<Screen> screen;
		/**
		*Stores the applications audio systems.
		*/
		static std::shared_ptr<AudioManager> audioManager;
		/**
		*Stores the engines component systems.
		*/
		static std::vector<std::shared_ptr<SystemBase>> componentSystems;

		/**
		*Binds the core engine systems.
		*/
		static void BindCoreSystems();
	public:

		/**
		*Stores the current entities from the loaded scene.
		*/
		static std::vector<Entity> entities;

		/**
		*Initializes the engine.
		*/
		static std::shared_ptr<Application> InitializeEngine(std::string _windowName, int _windowWidth, int _windowHeight);
		/**
		*The main engine loop.
		*/
		static void MainEngineLoop();
		/**
		*Creates and returns an entity.
		*/
		static 	Entity* CreateEntity();
		/**
		*Creates and binds the system of type T.
		*/
		template<typename T>
		static std::shared_ptr<T> BindSystem()
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
			system->InitializeSystem();
			componentSystems.push_back(system);
			return system;
		}
	};
}