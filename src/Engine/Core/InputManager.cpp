#include "InputManager.h"

#include <algorithm>
#include <fstream>

#include "Application.h"
#include "Graphics\Renderer.h"

#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../imgui/backends/imgui_impl_opengl3.h"

void InputManager::RemoveKeyFromOldDownKeys(SDL_Scancode key)
{
	for (int i = 0; i < m_vOldDownKeys.size(); i++)
	{
		if (m_vOldDownKeys.at(i) == key)
		{
			m_vOldDownKeys.erase(m_vOldDownKeys.begin() + i);
			return;
		}
	}
}

void InputManager::SaveKeybindings()
{
	nlohmann::json keybinds;

	for (int i = 0; i < m_vKeybinds.size(); i++)
	{
		keybinds["Keybinds"][m_vKeybinds.at(i).m_keybindIdentifier] = m_vKeybinds.at(i).WriteKeybindToJson();
	}

	std::fstream keybindFile("Keybinds.json", 'w');
	keybindFile << keybinds.dump();
	keybindFile.close();

	LogInfo("Saved keybindings.");
}

void InputManager::LoadKeybindings()
{
	std::ifstream keybindFile("Keybinds.json");
	if (!keybindFile.is_open())
	{
		LogInfo("Couldnt find keybinding file.");
		return;
	}

	nlohmann::json keybinds = nlohmann::json::parse(keybindFile);

	if (keybinds.find("Keybinds") != keybinds.end())
	{
		nlohmann::json binds = keybinds.at("Keybinds");

		for (auto it : binds.items())
		{
			Keybind* keybind = FindKeybind(it.key());
			if (keybind)
				keybind->FromJson(binds[it.key()]);
		}
	}

	LogInfo("Loaded keybindings.");
}

void InputManager::HandleGeneralInput()
{
	SDL_Event e = { 0 };

	while (SDL_PollEvent(&e) != 0)
	{
		ImGui_ImplSDL2_ProcessEvent(&e);

		GetMousePosition();

		if (e.type == SDL_QUIT)
		{
			Application::GetApplication()->StopApplication();
		}

		if (e.type == SDL_KEYDOWN)
		{
			m_vDownKeys.push_back(e.key.keysym.scancode);
		}
		else if (e.type == SDL_KEYUP)
		{
			m_vUpKeys.push_back(e.key.keysym.scancode);
			RemoveKeyFromOldDownKeys(e.key.keysym.scancode);
		}

		if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			m_vDownMouseButtons.push_back(e.button.button);
		}
		else if (e.type == SDL_MOUSEBUTTONUP)
		{
			m_vUpMouseButtons.push_back(e.button.button);
		}
	}
}

void InputManager::AddKeybind(Keybind keybind)
{
	for (int i = 0; i < m_vKeybinds.size(); i++)
	{
		if (m_vKeybinds.at(i).m_keybindIdentifier == keybind.m_keybindIdentifier)
		{
			LogError(FormatString("Tried to add keybind %s but it already exists.", keybind.m_keybindIdentifier));
			return;
		}
	}

	m_vKeybinds.push_back(keybind);
}

void InputManager::ChangeKeybind(std::string _keybind, int index, Input newInput)
{
	for (int i = 0; i < m_vKeybinds.size(); i++)
	{
		for (int o = 0; o < m_vKeybinds.at(i).m_vKeybindKeys.size(); o++)
		{
			if (m_vKeybinds.at(i).m_vKeybindKeys.at(o) == newInput)
			{
				LogError(FormatString("Tried to rebind keybind %s to already bound input.", _keybind));
				return;
			}
		}
	}

	for (int i = 0; i < m_vKeybinds.size(); i++)
	{
		if (m_vKeybinds.at(i).m_keybindIdentifier == _keybind)
		{
			m_vKeybinds.at(i).m_vKeybindKeys.at(index) = newInput;
			LogInfo(FormatString("Changed input %d for keybind %s.", index, _keybind));
			break;
		}
	}
}

Keybind* InputManager::FindKeybind(std::string _keybind)
{
	for (int i = 0; i < m_vKeybinds.size(); i++)
	{
		if (m_vKeybinds.at(i).m_keybindIdentifier == _keybind)
		{
			return &m_vKeybinds.at(i);
		}
	}

	return nullptr;
}

bool InputManager::GetKeybind(std::string _keybind)
{
	bool isKeybind = false;
	Keybind* keybind = FindKeybind(_keybind);

	if(keybind)
	{
		for (int o = 0; o < keybind->m_vKeybindKeys.size(); o++)
		{
			if (keybind->m_vKeybindKeys.at(o).m_keycode != SDLK_UNKNOWN)
				isKeybind = GetKey(keybind->m_vKeybindKeys.at(o).m_keycode);
			else if (keybind->m_vKeybindKeys.at(o).m_iMouseButton != -1)
				isKeybind = GetMouseButton(keybind->m_vKeybindKeys.at(o).m_iMouseButton);

			if (isKeybind)
				return isKeybind;
		}
	}

	return isKeybind;
}

bool InputManager::GetKeybindDown(std::string _keybind)
{
	bool isKeybind = false;
	Keybind* keybind = FindKeybind(_keybind);

	if (keybind)
	{
		for (int o = 0; o < keybind->m_vKeybindKeys.size(); o++)
		{
			if (keybind->m_vKeybindKeys.at(o).m_keycode != SDLK_UNKNOWN)
				isKeybind = GetKeyDown(keybind->m_vKeybindKeys.at(o).m_keycode);
			else if (keybind->m_vKeybindKeys.at(o).m_iMouseButton != -1)
				isKeybind = GetMouseButtonDown(keybind->m_vKeybindKeys.at(o).m_iMouseButton);

			if (isKeybind)
				return isKeybind;
		}

		if (isKeybind)
			return isKeybind;
	}

	return isKeybind;
}

bool InputManager::GetKeybindUp(std::string _keybind)
{
	bool isKeybind = false;
	Keybind* keybind = FindKeybind(_keybind);

	if (keybind)
	{
		for (int o = 0; o < keybind->m_vKeybindKeys.size(); o++)
		{
			if (keybind->m_vKeybindKeys.at(o).m_keycode != SDLK_UNKNOWN)
				isKeybind = GetKeyUp(keybind->m_vKeybindKeys.at(o).m_keycode);
			else if (keybind->m_vKeybindKeys.at(o).m_iMouseButton != -1)
				isKeybind = GetMouseButtonUp(keybind->m_vKeybindKeys.at(o).m_iMouseButton);

			if (isKeybind)
				return isKeybind;
		}

		if (isKeybind)
			return isKeybind;
	}

	return isKeybind;
}

bool InputManager::GetKey(SDL_Keycode _key)
{
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_GetScancodeFromKey(_key)])
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool InputManager::GetKeyDown(SDL_Keycode _key)
{
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (std::find(m_vDownKeys.begin(), m_vDownKeys.end(), SDL_GetScancodeFromKey(_key)) != m_vDownKeys.end())
	{
		if (std::find(m_vOldDownKeys.begin(), m_vOldDownKeys.end(), SDL_GetScancodeFromKey(_key)) == m_vOldDownKeys.end())
		{
			m_vOldDownKeys.push_back(SDL_GetScancodeFromKey(_key));
			return true;
		}
	}

	return false;
}

bool InputManager::GetKeyUp(SDL_Keycode _key)
{
	for (int i = 0; i < m_vUpKeys.size(); i++)
	{
		if (m_vUpKeys.at(i) == SDL_GetScancodeFromKey(_key))
		{
			return true;
		}
	}

	return false;
}

bool InputManager::GetMouseButton(int _button)
{
	if (SDL_GetMouseState(&m_iMouseX, &m_iMouseY) & SDL_BUTTON(_button))
	{
		return true;
	}

	return false;
}

bool InputManager::GetMouseButtonDown(int _button)
{
	for (int i = 0; i < m_vDownMouseButtons.size(); i++)
	{
		if (m_vDownMouseButtons.at(i) == _button)
		{
			if (std::find(m_vOldMouseButtons.begin(), m_vOldMouseButtons.end(), _button) == m_vOldMouseButtons.end())
			{
				return true;
			}
		}
	}

	return false;
}

bool InputManager::GetMouseButtonUp(int _button)
{
	for (int i = 0; i < m_vUpMouseButtons.size(); i++)
	{
		if (m_vUpMouseButtons.at(i) == _button)
		{
			return true;
		}
	}

	return false;
}

void InputManager::GetMousePosition()
{
	SDL_GetMouseState(&m_iMouseX, &m_iMouseY);
}

void InputManager::ClearFrameInputs()
{
	m_vOldMouseButtons = m_vDownMouseButtons;

	m_vDownKeys.clear();
	m_vUpKeys.clear();
	m_vDownMouseButtons.clear();
	m_vUpMouseButtons.clear();
}