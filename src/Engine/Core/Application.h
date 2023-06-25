#pragma once
#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <iostream>

#include <SDL/SDL.h>

#include "Renderer.h"
#include "../Networking/NetworkManager.h"
#include "AudioManager.h"
#include "ThreadingManager.h"
#include "PerformanceStats.h"
#include "Logger.h"

#include "ResourceManager.h"
#include "../Resource/IniFile.h"
#include "../Resource/ShaderProgram.hpp"

#include "../ECS/Entity.hpp"
#include "../ECS/System.hpp"
#include "../ECS/Behaviour.hpp"

#include "DebugUI.hpp"
#include "UI.h"

class Application
{
    friend class InputManager;
	friend class SceneManager;

private:
    
	static std::weak_ptr<Application> m_self;

    Renderer* m_gameRenderer;
	AudioManager* m_audioManager;
	NetworkManager* m_networkManager;
    ThreadingManager* m_threadManager;
    ResourceManager* m_resourceManager;
    Logger* m_logger;

    std::shared_ptr<IniFile> m_mainIniFile;

    static bool m_bLoop;

    PerformanceStats* m_pStats;

    static std::vector<std::shared_ptr<SystemBase>> m_vComponentSystems;
	static std::deque<Entity*> m_vDeletionEntities;
	static std::vector<Entity> m_vEntities;

	static std::vector<std::shared_ptr<UISystem>> m_vUiSystems;
	static std::vector<std::shared_ptr<Behaviour>> m_vBehaviours;
	static std::vector<std::shared_ptr<DebugUI>> m_vDebugUIs;

	std::string GetUniqueEntityID();

	static std::vector<Entity>& GetEntityList() { return m_vEntities; }
	static std::vector<std::shared_ptr<SystemBase>> GetSystemList() { return m_vComponentSystems; }

public:

	static std::shared_ptr<ShaderProgram> m_mainShaderProgram;
	static std::shared_ptr<ShaderProgram> m_uiShaderProgram;
	static std::shared_ptr<ShaderProgram> m_uiTextProgram;

    static std::shared_ptr<Application> StartApplication(const std::string _windowName);
    void LoadSettings();
    void MainLoop();
    void CleanupApplication();

	static Entity* CreateEntity();
	static Entity* CreateEntity(std::string _desiredID);
	static void DeleteEntity(std::string _ID);
	static Entity* GetEntity(std::string _ID);
	void CleanupDeletionEntities();

	static void ClearLoadedScene();

	static std::shared_ptr<SystemBase> GetSystemFromID(std::string _ID);

	template<typename T>
	static std::shared_ptr<T> BindSystem(SystemUsage _usage, std::string _ID)
	{
		std::shared_ptr<T> temp;
		for (size_t sys = 0; sys < m_vComponentSystems.size(); sys++)
		{
			temp = std::dynamic_pointer_cast<T>(m_vComponentSystems.at(sys));
			if (temp)
			{
				std::cout << "System is already bound!!" << std::endl;
				return temp;
			}
		}

		std::shared_ptr<T> system = std::make_shared<T>();
		system->self = system;
		system->systemUsage = _usage;
		system->useThreads = false;
		system->InitializeSystem(_ID);
		m_vComponentSystems.push_back(system);
		return system;
	}

	template<typename T>
	static std::shared_ptr<T> BindSystem(SystemUsage _usage, int _componentsPerThread)
	{
		std::shared_ptr<T> temp;
		for (size_t sys = 0; sys < m_vComponentSystems.size(); sys++)
		{
			temp = std::dynamic_pointer_cast<T>(m_vComponentSystems.at(sys));
			if (temp)
			{
				std::cout << "System is already bound!!" << std::endl;
				return temp;
			}
		}

		std::shared_ptr<T> system = std::make_shared<T>();
		system->self = system;
		system->systemUsage = _usage;
		system->useThreads = true;
		system->maxComponentsPerThread = _componentsPerThread;
		system->InitializeSystem();
		m_vComponentSystems.push_back(system);
		return system;
	}

	template<typename T>
	static std::shared_ptr<T> BindUISystem(const std::string _UILayout)
	{
		std::shared_ptr<T> sys = std::make_shared<T>(_UILayout);
		sys->InitializeUI();
		sys->InitializeSpecificElements();

		m_vUiSystems.push_back(sys);
		return sys;
	}

	template<typename T>
	static std::shared_ptr<T> BindDebugUI()
	{
		std::shared_ptr<T> sys = std::make_shared<T>();
		sys->InitializeInterface();
		m_vDebugUIs.push_back(sys);
		return sys;
	}

	template<typename T>
	static std::shared_ptr<T> BindBehaviour()
	{
		std::shared_ptr<T> beh = std::make_shared<T>();
		beh->Start();
		m_vBehaviours.push_back(beh);
		return beh;
	}

	static void CreateNetworkManager(const int& _mode);
};

#endif