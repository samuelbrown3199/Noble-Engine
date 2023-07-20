#pragma once
#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <iostream>

#include <SDL/SDL.h>

#include "Graphics/Renderer.h"
#include "../Networking/NetworkManager.h"
#include "AudioManager.h"
#include "ThreadingManager.h"
#include "PerformanceStats.h"
#include "Logger.h"

#include "ResourceManager.h"
#include "../Resource/IniFile.h"

#include "../ECS/Entity.hpp"
#include "../ECS/System.hpp"
#include "../ECS/Behaviour.hpp"

#include "DebugUI.hpp"

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

	VkDescriptorPool m_imguiPool;

    std::shared_ptr<IniFile> m_mainIniFile;

	static bool m_bEntitiesDeleted;
    static bool m_bLoop;

    PerformanceStats* m_pStats;

    static std::vector<std::shared_ptr<SystemBase>> m_vComponentSystems;
	static std::deque<Entity*> m_vDeletionEntities;
	static std::vector<Entity> m_vEntities;

	static std::vector<std::shared_ptr<DebugUI>> m_vDebugUIs;

	std::string GetUniqueEntityID();
	void InitializeImGui();

public:

    static std::shared_ptr<Application> StartApplication(const std::string _windowName);
	static void StopApplication() { m_bLoop = false; }

	void RegisterCoreKeybinds();
    void LoadSettings();
    void MainLoop();
    void CleanupApplication();

	static bool GetEntitiesDeleted() { return m_bEntitiesDeleted; }

	static Entity* CreateEntity();
	static Entity* CreateEntity(std::string _desiredID, std::string _name);
	static void DeleteEntity(std::string _ID);
	static Entity* GetEntity(std::string _ID);
	void CleanupDeletionEntities();

	static std::vector<Entity>& GetEntityList() { return m_vEntities; }
	static std::vector<std::shared_ptr<SystemBase>> GetSystemList() { return m_vComponentSystems; }

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
				Logger::LogError("System is already bound.", 2);
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
	static std::shared_ptr<T> BindSystem(SystemUsage _usage, std::string _ID, int _componentsPerThread)
	{
		std::shared_ptr<T> temp;
		for (size_t sys = 0; sys < m_vComponentSystems.size(); sys++)
		{
			temp = std::dynamic_pointer_cast<T>(m_vComponentSystems.at(sys));
			if (temp)
			{
				Logger::LogError("System is already bound.", 2);
				return temp;
			}
		}

		std::shared_ptr<T> system = std::make_shared<T>();
		system->self = system;
		system->systemUsage = _usage;
		system->useThreads = true;
		system->maxComponentsPerThread = _componentsPerThread;
		system->InitializeSystem(_ID);
		m_vComponentSystems.push_back(system);
		return system;
	}

	template<typename T>
	static std::shared_ptr<T> BindDebugUI()
	{
		std::shared_ptr<T> sys = std::make_shared<T>();
		sys->InitializeInterface();
		m_vDebugUIs.push_back(sys);
		return sys;
	}

	static void CreateNetworkManager(const int& _mode);
};

#endif