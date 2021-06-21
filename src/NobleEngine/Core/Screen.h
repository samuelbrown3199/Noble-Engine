#ifndef SCREEN_H_
#define SCREEN_H_

#include <string>

#include<glm/glm.hpp>
#include <glm/ext.hpp>
#include <SDL/SDL.h>
#include <GL/glew.h>

namespace NobleCore
{
	/**
	*Screen holds information and functions related to the game window.
	*/
	class Screen
	{
		friend class Renderer;

	private:
		/**
		* Stores the window name.
		*/
		static std::string windowName;
		/**
		*Stores the screen Width.
		*/
		static int screenWidth;
		/**
		*Stores the screen height.
		*/
		static int screenHeight;
		/**
		*Stores a pointer to the application window.
		*/
		static SDL_Window* window;
	public:

		/**
		*Constructor for the screen class.
		*/
		Screen(std::string _windowName, int _width, int _height);
		/**
		*Updates the screen size.
		*/
		static void UpdateScreenSize();
		/**
		*This is used to set the window in and out of full screen. 
		*Flags SDL_WINDOW_FULLSCREEN, SDL_WINDOW_FULLSCREEN_DESKTOP or 0 for Windowed Mode.
		*/
		static void SetWindowFullScreen(Uint32 _flags);
		/**
		* Returns the window name.
		*/
		static std::string GetWindowName();
		/**
		*Returns the screen width.
		*/
		static int GetScreenWidth();
		/**
		*Returns the screen height.
		*/
		static int GetScreenHeight();
		/**
		*Returns the SDL window pointer.
		*/
		static SDL_Window* GetWindow();
	};
}

#endif