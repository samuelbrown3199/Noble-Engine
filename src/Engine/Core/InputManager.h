#pragma once
#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include <iostream>
#include <vector>
#include <array>

#include <SDL/SDL.h>
#include <glm/glm.hpp>

#include <nlohmann/json.hpp>

#include "Logger.h"
#include "../Useful.h"

struct Controller
{
	SDL_Joystick* m_joystick;

	float m_fLeftStickX = 0.0f, m_fLeftStickY = 0.0f;
	float m_fRightStickX = 0.0f, m_fRightStickY = 0.0f;
};

struct Input
{
private:

	void ResetInputs()
	{
		m_iMouseButton = -1;
		m_keycode = SDLK_UNKNOWN;
	}

public:

	SDL_Keycode m_keycode = SDLK_UNKNOWN;
	int m_iMouseButton = -1;

	Input() {}

	bool operator==(Input& other)
	{
		if (other.m_keycode != SDLK_UNKNOWN)
			return other.m_keycode == m_keycode;

		if(other.m_iMouseButton != -1)
			return other.m_iMouseButton == m_iMouseButton;

		return false;
	}

	Input(SDL_Keycode keycode, int mouseButton)
	{
		if (keycode != SDLK_UNKNOWN && mouseButton != -1)
			Logger::LogError("Trying to register an invalid Input.", 2);

		m_keycode = keycode;
		m_iMouseButton = mouseButton;
	}

	void SetInput(SDL_Keycode keycode, int mouseButton)
	{
		if (keycode != SDLK_UNKNOWN && mouseButton != -1)
			Logger::LogError("Trying to change to an invalid Input.", 2);

		ResetInputs();
		m_keycode = keycode;
		m_iMouseButton = mouseButton;
	}

	nlohmann::json WriteInputToJson()
	{
		nlohmann::json data;
		
		if (m_keycode != SDLK_UNKNOWN)
		{
			data["InputType"] = "Keyboard";
			data["InputValue"] = m_keycode;
		}
		else if (m_iMouseButton != -1)
		{
			data["InputType"] = "MouseButton";
			data["InputValue"] = m_iMouseButton;
		}
		else
		{
			data["InputType"] = "UNKNOWN";
			data["InputValue"] = -1;
		}

		return data;
	}

	void FromJson(const nlohmann::json& j)
	{
		if (j["InputType"] == "Keyboard")
		{
			m_keycode = j["InputValue"];
		}
		else if (j["InputType"] == "MouseButton")
		{
			m_iMouseButton = j["InputValue"];
		}
	}
};

struct Keybind
{
	std::string m_keybindIdentifier;
	std::array<Input, 3> m_vKeybindKeys;

	Keybind(std::string id, std::array<Input, 3> keys)
	{
		m_keybindIdentifier = id;
		m_vKeybindKeys = keys;
	}

	void ChangeKeybind(int index, Input newInput)
	{
		if(index < 0 || index > m_vKeybindKeys.size()-1)
			Logger::LogError("Trying to change a keybind at an invalid index.", 2);

		m_vKeybindKeys.at(index) = newInput;
	}

	nlohmann::json WriteKeybindToJson()
	{
		nlohmann::json data;

		for (int i = 0; i < m_vKeybindKeys.size(); i++)
		{
			std::string key = FormatString("Input%d", i);
			data[key] = m_vKeybindKeys.at(i).WriteInputToJson();
		}

		return data;
	}

	void FromJson(const nlohmann::json& j)
	{
		int i = 0;
		for (auto it : j.items())
		{
			m_vKeybindKeys.at(i).FromJson(j[it.key()]);
			i++;
		}
	}
};

/**
*Handles input for the engine.
*/
class InputManager
{
private:

	static Controller m_controller;

	static std::vector<Keybind> m_vKeybinds;

	static void RemoveKeyFromOldDownKeys(SDL_Scancode key);

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

	static void SaveKeybindings();
	static void LoadKeybindings();

	static void HandleGeneralInput();
	
	static void AddKeybind(Keybind keybind);
	static void ChangeKeybind(std::string _keybind, int index, Input newInput);

	static Keybind* FindKeybind(std::string _keybind);
	static bool GetKeybind(std::string _keybind);
	static bool GetKeybindDown(std::string _keybind);
	static bool GetKeybindUp(std::string _keybind);

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
};

#endif

