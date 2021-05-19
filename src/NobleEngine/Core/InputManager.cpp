#include "InputManager.h"

#include <algorithm>

#include "Application.h"

namespace NobleCore
{
	int InputManager::mouseX = 0;
	int InputManager::mouseY = 0;

	std::vector<SDL_Scancode> InputManager::downKeys = std::vector<SDL_Scancode>(), InputManager::oldDownKeys = std::vector<SDL_Scancode>();
	std::vector<SDL_Scancode> InputManager::upKeys = std::vector<SDL_Scancode>();
	std::vector<int> InputManager::downMouseButtons = std::vector<int>(), InputManager::oldMouseButtons = std::vector<int>();
	std::vector<int> InputManager::upMouseButtons = std::vector<int>();

	void InputManager::HandleGeneralInput()
	{
		SDL_Event e = { 0 };

		while (SDL_PollEvent(&e) != 0)
		{
			GetMousePosition();

			if (e.type == SDL_QUIT)
			{
				Application::loop = false;
			}

			if (e.type == SDL_KEYDOWN)
			{
				downKeys.push_back(e.key.keysym.scancode);
			}
			else if (e.type == SDL_KEYUP)
			{
				upKeys.push_back(e.key.keysym.scancode);
			}

			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT))
				{
					downMouseButtons.push_back(0);
				}
				if (SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON(SDL_BUTTON_RIGHT))
				{
					downMouseButtons.push_back(1);
				}
				if (SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON(SDL_BUTTON_MIDDLE))
				{
					downMouseButtons.push_back(2);
				}
			}
			else
			{
				if (SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT))
				{
					upMouseButtons.push_back(0);
				}
				if (SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON(SDL_BUTTON_RIGHT))
				{
					upMouseButtons.push_back(1);
				}
				if (SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON(SDL_BUTTON_MIDDLE))
				{
					upMouseButtons.push_back(2);
				}
			}
		}
	}
	bool InputManager::GetKey(SDL_Keycode key)
	{
		const Uint8* state = SDL_GetKeyboardState(NULL);
		if (state[SDL_GetScancodeFromKey(key)])
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool InputManager::GetKeyDown(SDL_Keycode key)
	{
		for (int i = 0; i < downKeys.size(); i++)
		{
			if (downKeys.at(i) == SDL_GetScancodeFromKey(key))
			{
				if (std::find(oldDownKeys.begin(), oldDownKeys.end(), SDL_GetScancodeFromKey(key)) == oldDownKeys.end())
				{
					return true;
				}
			}
		}

		return false;
	}

	bool InputManager::GetKeyUp(SDL_Keycode key)
	{
		for (int i = 0; i < upKeys.size(); i++)
		{
			if (upKeys.at(i) == SDL_GetScancodeFromKey(key))
			{
				return true;
			}
		}

		return false;
	}

	bool InputManager::GetMouseButton(int button)
	{
		if (button == 0)
		{
			if (SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT))
			{
				return true;
			}
		}
		else if (button == 1)
		{
			if (SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON(SDL_BUTTON_RIGHT))
			{
				return true;
			}
		}
		else if (button == 2)
		{
			if (SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON(SDL_BUTTON_MIDDLE))
			{
				return true;
			}
		}

		return false;
	}

	bool InputManager::GetMouseButtonDown(int button)
	{
		for (int i = 0; i < downMouseButtons.size(); i++)
		{
			if (downMouseButtons.at(i) == button)
			{
				if (std::find(downMouseButtons.begin(), downMouseButtons.end(), button) == downMouseButtons.end())
				{
					return true;
				}
			}
		}

		false;
	}

	bool InputManager::GetMouseButtonUp(int button)
	{
		for (int i = 0; i < upMouseButtons.size(); i++)
		{
			if (upMouseButtons.at(i) == button)
			{
				return true;
			}
		}

		return false;
	}

	void InputManager::GetMousePosition()
	{
		SDL_GetMouseState(&mouseX, &mouseY);
	}

	void InputManager::ClearFrameInputs()
	{
		oldDownKeys = downKeys;
		oldMouseButtons = downMouseButtons;

		downKeys.clear();
		upKeys.clear();
		downMouseButtons.clear();
		upMouseButtons.clear();
	}
}