#pragma once

#include <iostream>

#include <unordered_map>
#include <SDL/SDL.h>

#include "Graphics/Renderer.h"
#include "AudioManager.h"
#include "ThreadingManager.h"
#include "PerformanceStats.h"
#include "Logger.h"
#include "ResourceManager.h"

#include "../Core/EngineResources/IniFile.h"

#include "ToolUI.h"

struct Entity;
class InputManager;
class NobleRegistry;
class SceneManager;
class ProjectFile;
class NobleDLL;
struct Command;

class Editor
{
public:

	ProjectFile* m_projectFile = nullptr;

	virtual void CleanupEditor() = 0;

	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;

	virtual void HandleQuit() = 0;

	virtual void SetProjectFile(ProjectFile* projectFile) = 0;

	virtual void PushCommand(Command* command) = 0;
	virtual void ConsoleLog(std::string message) = 0;
	virtual void SetSelectedEntity(std::string ID) = 0;
	virtual void SetSelectedResource(std::string localPath) = 0;

	virtual void CreateEntityCopy(Entity* entity) = 0;

	virtual void AddComponentMeasurement(std::string name) = 0;
};

class Application
{
    friend class InputManager;
	friend class SceneManager;

private:

	std::mutex m_criticalSection;

	bool m_bPlayMode;

	NobleRegistry* m_registry;
    Renderer* m_gameRenderer;
	AudioManager* m_audioManager;
    ThreadingManager* m_threadManager;
    ResourceManager* m_resourceManager;
    Logger* m_logger;
	InputManager* m_inputManager;
	SceneManager* m_sceneManager;

	Editor* m_editor;

	ProjectFile* m_projectFile;

    std::shared_ptr<IniFile> m_mainIniFile;

    bool m_bLoop;

	NobleDLL* m_gameDLL;

    PerformanceStats* m_pStats;

	std::deque<Entity*> m_vDeletionEntities;
	std::unordered_map<std::string, Entity> m_vEntities;

	std::vector<std::shared_ptr<ToolUI>> m_vToolUIs;

	std::string GetUniqueEntityID();

public:

	static std::weak_ptr<Application> m_self;

	static std::shared_ptr<Application> GetApplication() { return m_self.lock(); }
    static std::shared_ptr<Application> StartApplication(const std::string _windowName);

	void StopApplication();
	void ForceQuit();
	bool GetMainLoop();

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

	void DeleteEntity(std::string ID);
	Entity* GetEntity(std::string ID);
	void CleanupDeletionEntities();

	std::unordered_map<std::string, Entity>& GetEntityList() { return m_vEntities; }
	void ClearLoadedScene();

	NobleRegistry* GetRegistry() { return m_registry; }
	Renderer* GetRenderer() { return m_gameRenderer; }
	AudioManager* GetAudioManager() { return m_audioManager; }
	ThreadingManager* GetThreadingManager() { return m_threadManager; }
	ResourceManager* GetResourceManager() { return m_resourceManager; }
	Logger* GetLogger() { return m_logger; }
	InputManager* GetInputManager() { return m_inputManager; }
	SceneManager* GetSceneManager() { return m_sceneManager; }

	void SetProjectFile(std::string path);
	ProjectFile* GetProjectFile() { return m_projectFile; }

	void RegisterDefaultComponents();
	void ResetRegistries();

	Editor* GetEditor() { return m_editor; }
	void PushCommand(Command* command);
	void ConsoleLog(std::string message);

	template<typename T>
	std::shared_ptr<T> BindDebugUI()
	{
		std::shared_ptr<T> sys = std::make_shared<T>();
		sys->InitializeInterface();
		m_vToolUIs.push_back(sys);
		return sys;
	}
};