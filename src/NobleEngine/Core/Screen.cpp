#include "Screen.h"

#include <iostream>
#include <exception>

namespace NobleCore
{
	int Screen::screenWidth = 0;
	int Screen::screenHeight = 0;
	SDL_Window* Screen::window;

	Screen::Screen(std::string _windowName, int _width, int _height)
	{
		screenWidth = _width;
		screenHeight = _height;

		window = SDL_CreateWindow(_windowName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
		if (!window)
		{
			std::cout << "Failed to create window " << SDL_GetError() << std::endl;
			throw std::exception();
		}
	}

	void Screen::SetWindowFullScreen(Uint32 _flags)
	{
		SDL_SetWindowFullscreen(window, _flags);
	}

	int Screen::GetScreenWidth()
	{
		return screenWidth;
	}

	int Screen::GetScreenHeight()
	{
		return screenHeight;
	}
}