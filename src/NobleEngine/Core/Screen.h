
#include <string>

#include <SDL/SDL.h>

namespace NobleCore
{
	/**
	*Screen holds information and functions related to the game window.
	*/
	class Screen
	{
	private:
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
		*This is used to set the window in and out of full screen. 
		*Flags SDL_WINDOW_FULLSCREEN, SDL_WINDOW_FULLSCREEN_DESKTOP or 0.
		*/
		static void SetWindowFullScreen(Uint32 _flags);
		/**
		*Returns the screen width.
		*/
		static int GetScreenWidth();
		/**
		*Returns the screen height.
		*/
		static int GetScreenHeight();
	};
}