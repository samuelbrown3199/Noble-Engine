#include "Screen.h"

#include <iostream>
#include <exception>

namespace NobleCore
{
	std::string Screen::windowName;
	int Screen::screenWidth = 0;
	int Screen::screenHeight = 0;
	SDL_Window* Screen::window;

	Screen::Screen(std::string _windowName, int _width, int _height)
	{
		windowName = _windowName;
		screenWidth = _width;
		screenHeight = _height;
	}

	void Screen::UpdateScreenSize()
	{
		SDL_GetWindowSize(window, &screenWidth, &screenHeight);
		glViewport(0, 0, screenWidth, screenHeight);
	}

	void Screen::SetWindowFullScreen(Uint32 _flags)
	{
		SDL_SetWindowFullscreen(window, _flags);
	}

	std::string Screen::GetWindowName()
	{
		return windowName;
	}

	int Screen::GetScreenWidth()
	{
		return screenWidth;
	}

	int Screen::GetScreenHeight()
	{
		return screenHeight;
	}

	SDL_Window* Screen::GetWindow()
	{
		return window;
	}
}