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
class SceneManager;
class ProjectFile;

class Editor
{
public:

	ProjectFile* m_projectFile = nullptr;

	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;

	virtual void HandleQuit() = 0;

	virtual void SetProjectFile(ProjectFile* projectFile) = 0;
};

class Application
{
    friend class InputManager;
	friend class SceneManager;

private:
    
	static std::weak_ptr<Application> m_self;

	bool m_bPlayMode;

	NobleRegistry* m_registry;
    Renderer* m_gameRenderer;
	AudioManager* m_audioManager;
    ThreadingManager* m_threadManager;
    ResourceManager* m_resourceManager;
    Logger* m_logger;
	SceneManager* m_sceneManager;

	Editor* m_editor;

	ProjectFile* m_projectFile;

    std::shared_ptr<IniFile> m_mainIniFile;

    bool m_bLoop;

    PerformanceStats* m_pStats;

	std::deque<Entity*> m_vDeletionEntities;
	std::vector<Entity> m_vEntities;

	std::vector<std::shared_ptr<ToolUI>> m_vToolUIs;

	std::string GetUniqueEntityID();

public:

	static std::shared_ptr<Application> GetApplication() { return m_self.lock(); }
    static std::shared_ptr<Application> StartApplication(const std::string _windowName);

	void StopApplication();
	void ForceQuit();

	void SetPlayMode(bool play);
	bool GetPlayMode() { return m_bPlayMode; }

	void BindEditor(Editor* editor);

	void RegisterCoreKeybinds();
    void LoadSettings();
    void MainLoop();
    void CleanupApplication();

	PerformanceStats* GetPerformanceStats() { return m_pStats; }

	Entity* CreateEntity();
	Entity* CreateEntity(std::string _desiredID, std::string _name, std::string _parentID);

	void LinkChildEntities();

	int GetEntityIndex(std::string _ID);
	void DeleteEntity(int index);
	Entity* GetEntity(int index);
	void CleanupDeletionEntities();

	std::vector<Entity>& GetEntityList() { return m_vEntities; }
	void ClearLoadedScene();

	NobleRegistry* GetRegistry() { return m_registry; }
	Renderer* GetRenderer() { return m_gameRenderer; }
	AudioManager* GetAudioManager() { return m_audioManager; }
	ThreadingManager* GetThreadingManager() { return m_threadManager; }
	ResourceManager* GetResourceManager() { return m_resourceManager; }
	Logger* GetLogger() { return m_logger; }
	SceneManager* GetSceneManager() { return m_sceneManager; }

	void SetProjectFile(std::string path);
	ProjectFile* GetProjectFile() { return m_projectFile; }

	template<typename T>
	std::shared_ptr<T> BindDebugUI()
	{
		std::shared_ptr<T> sys = std::make_shared<T>();
		sys->InitializeInterface();
		m_vToolUIs.push_back(sys);
		return sys;
	}
};