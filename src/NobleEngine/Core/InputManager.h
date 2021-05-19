#pragma once
#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include <iostream>
#include <vector>

#include <SDL/SDL.h>

namespace NobleCore
{
	/**
	*Handles input for the engine.
	*/
	class InputManager
	{
	public:
		/**
		*Stores the mouse x and y positions.
		*/
		static int mouseX, mouseY;
		static std::vector<SDL_Scancode> downKeys, oldDownKeys;
		static std::vector<SDL_Scancode> upKeys;
		static std::vector<int> downMouseButtons, oldMouseButtons;
		static std::vector<int> upMouseButtons;

		static void HandleGeneralInput();

		/**
		*Checks if a key is down. Uses SDL keycodes which can be found here: https://wiki.libsdl.org/SDL_Scancode
		*/
		static bool GetKey(SDL_Keycode _key);
		/**
		*Checks if the key was pressed this frame.
		*/
		static bool GetKeyDown(SDL_Keycode _key);
		/**
		*Checks if the key was released this frame.
		*/
		static bool GetKeyUp(SDL_Keycode _key);
		/**
		*Checks if a mouse button is down. Button 0 is left, 1 is right, 2 is middle.
		*/
		static bool GetMouseButton(int _button);
		/**
		*Checks if the mouse button has been pressed this frame.
		*/
		static bool GetMouseButtonDown(int button);
		/**
		*Checks if the mouse button has been released this frame.
		*/
		static bool GetMouseButtonUp(int button);
		/**
		*Gets the mouse position and stores it in the mouse position variables.
		*/
		static void GetMousePosition();
		/**
		*Clears the keys that are currently up and down.
		*/
		static void ClearFrameInputs();
	};
}
#endif

