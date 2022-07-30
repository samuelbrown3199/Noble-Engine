#pragma once
#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include <iostream>
#include <vector>

#include <SDL/SDL.h>
#include <glm/glm.hpp>

	/**
	*Handles input for the engine.
	*/
class InputManager
{
private:

	static bool m_bCurrentlyInputtingText;

public:
	/**
	*Stores the mouse x and y positions.
	*/
	static int m_iMouseX, m_iMouseY;
	static glm::vec2 m_iWorldMousePos;
	static std::vector<SDL_Scancode> m_vDownKeys, m_vOldDownKeys;
	static std::vector<SDL_Scancode> m_vUpKeys;
	static std::vector<int> m_vDownMouseButtons, m_vOldMouseButtons;
	static std::vector<int> m_vUpMouseButtons;
	static std::string m_sInputText;

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
	static bool GetMouseButtonDown(int _button);
	/**
	*Checks if the mouse button has been released this frame.
	*/
	static bool GetMouseButtonUp(int _button);
	/**
	*Gets the mouse position and stores it in the mouse position variables.
	*/
	static void GetMousePosition();
	/**
	*Clears the keys that are currently up and down.
	*/
	static void ClearFrameInputs();

	static void StartTextInput();
	static void StopTextInput();
};

#endif

