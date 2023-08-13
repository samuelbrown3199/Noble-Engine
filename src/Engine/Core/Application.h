#pragma once

#include <iostream>

#include <SDL/SDL.h>

#include "Graphics/Renderer.h"
#include "AudioManager.h"
#include "ThreadingManager.h"
#include "PerformanceStats.h"
#include "Logger.h"

#include "ResourceManager.h"
#include "../Resource/IniFile.h"

#include "../ECS/Behaviour.hpp"

#include "DebugUI.hpp"

struct Entity;
class NobleRegistry;

class Application
{
    friend class InputManager;
	friend class SceneManager;

private:
    
	static std::weak_ptr<Application> m_self;

	NobleRegistry* m_registry;
    Renderer* m_gameRenderer;
	AudioManager* m_audioManager;
    ThreadingManager* m_threadManager;
    ResourceManager* m_resourceManager;
    Logger* m_logger;

	VkDescriptorPool m_imguiPool;

    std::shared_ptr<IniFile> m_mainIniFile;

	static bool m_bEntitiesDeleted;
    static bool m_bLoop;

    PerformanceStats* m_pStats;

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
	static void ClearLoadedScene();

	template<typename T>
	static std::shared_ptr<T> BindDebugUI()
	{
		std::shared_ptr<T> sys = std::make_shared<T>();
		sys->InitializeInterface();
		m_vDebugUIs.push_back(sys);
		return sys;
	}
};