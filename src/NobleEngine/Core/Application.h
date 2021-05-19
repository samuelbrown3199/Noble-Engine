#include "Screen.h"

#include <memory>
#include <exception>

#include <SDL/SDL.h>

namespace NobleCore
{
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
	public:
		/**
		*Initializes the engine.
		*/
		static std::shared_ptr<Application> InitializeEngine();
		/**
		*The main engine loop.
		*/
		static void MainEngineLoop();
	};
}