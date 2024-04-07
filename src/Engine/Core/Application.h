#pragma once

#include <iostream>

#include <SDL/SDL.h>

#include "Graphics/Renderer.h"
#include "AudioManager.h"
#include "ThreadingManager.h"
#include "PerformanceStats.h"
#include "Logger.h"

#include "ResourceManager.h"
#include "../Core/EngineResources/IniFile.h"

#include "ECS/Behaviour.hpp"

#include "ToolUI.hpp"

struct Entity;
class NobleRegistry;

class Editor
{
public:

	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;

	virtual void HandleQuit() = 0;
};

class Application
{
    friend class InputManager;
	friend class SceneManager;

private:
    
	static std::weak_ptr<Application> m_self;

	static bool m_bPlayMode;

	static NobleRegistry* m_registry;
    static Renderer* m_gameRenderer;
	AudioManager* m_audioManager;
    ThreadingManager* m_threadManager;
    ResourceManager* m_resourceManager;
    Logger* m_logger;

	Editor* m_editor;

    std::shared_ptr<IniFile> m_mainIniFile;

    static bool m_bLoop;

    static PerformanceStats* m_pStats;

	static std::deque<Entity*> m_vDeletionEntities;
	static std::vector<Entity> m_vEntities;

	static std::vector<std::shared_ptr<ToolUI>> m_vToolUIs;

	std::string GetUniqueEntityID();

public:

    static std::shared_ptr<Application> StartApplication(const std::string _windowName);
	static void StopApplication();
	static void ForceQuit();

	static void SetPlayMode(bool play);
	static bool GetPlayMode() { return m_bPlayMode; }

	void BindEditor(Editor* editor);

	void RegisterCoreKeybinds();
    void LoadSettings();
    void MainLoop();
    void CleanupApplication();

	static PerformanceStats* GetPerformanceStats() { return m_pStats; }

	static Entity* CreateEntity();
	static Entity* CreateEntity(std::string _desiredID, std::string _name, std::string _parentID);

	static void LinkChildEntities();

	static int GetEntityIndex(std::string _ID);
	static void DeleteEntity(int index);
	static Entity* GetEntity(int index);
	void CleanupDeletionEntities();

	static std::vector<Entity>& GetEntityList() { return m_vEntities; }
	static void ClearLoadedScene();

	static NobleRegistry* GetRegistry() { return m_registry; }
	static Renderer* GetRenderer() { return m_gameRenderer; }

	template<typename T>
	static std::shared_ptr<T> BindDebugUI()
	{
		std::shared_ptr<T> sys = std::make_shared<T>();
		sys->InitializeInterface();
		m_vToolUIs.push_back(sys);
		return sys;
	}
};