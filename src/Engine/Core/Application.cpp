#include "Application.h"
#include "InputManager.h"
#include "Registry.h"
#include "ProjectFile.h"
#include "SceneManager.h"
#include "NobleDLL.h"

#include "EngineComponents/Transform.h"
#include "EngineComponents/AudioListener.h"
#include "EngineComponents/AudioSource.h"
#include "EngineComponents/Camera.h"
#include "EngineComponents/MeshRenderer.h"
#include "EngineComponents/ScriptEmbedder.h"
#include "EngineComponents/Sprite.h"
#include "EngineComponents/Light.h"

#include "ECS/Entity.hpp"

#include "EngineBehaviours/DebugCam.h"

#include "../imgui/imgui.h"
#include "../imgui/implot.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../imgui/backends/imgui_impl_vulkan.h"
#include "../imgui/implot.h"

std::weak_ptr<Application> Application::m_self;

//----------------- Private Functions ----------------------

//----------------- Public Functions -----------------------

std::shared_ptr<Application> Application::StartApplication(const std::string _windowName)
{
	srand((unsigned int)time(NULL));

	std::shared_ptr<Application> rtn = std::make_shared<Application>();
	rtn->m_self = rtn;

	rtn->m_bLoop = true;

	rtn->m_pStats = new PerformanceStats();
	rtn->m_registry = new NobleRegistry();
	rtn->m_logger = new Logger();

#ifndef NDEBUG
	//We want to log initialization at least in debug mode.
	rtn->m_logger->m_bUseLogging = true;
#endif

	Logger::LogInformation("Starting Engine, Version: " + GetVersionInfoString());

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

	rtn->m_gameRenderer = new Renderer(_windowName);
	rtn->m_resourceManager = new ResourceManager();
	rtn->m_audioManager = new AudioManager();
	rtn->m_threadManager = new ThreadingManager();
	rtn->m_sceneManager = new SceneManager();

	rtn->RegisterDefaultComponents();

	rtn->RegisterCoreKeybinds();

	rtn->m_mainIniFile = std::make_shared<IniFile>(GetWorkingDirectory() + "\\game.ini");
	rtn->LoadSettings();

	std::vector<std::string> projectFiles = GetAllFilesOfType(GetWorkingDirectory(), ".npj");
	if (projectFiles.size() != 0)
	{
		rtn->SetProjectFile(projectFiles.at(0));
	}
	else
	{
		Logger::LogInformation("No project files found in working directory. Engine is running in editor mode.");
	}

	Logger::LogInformation("Engine started successfully");

	rtn->m_sceneManager->LoadDefaultScene();

	return rtn;
}

void Application::StopApplication()
{
	if(m_self.lock()->m_editor == nullptr)
		m_bLoop = false;
	else
	{
		m_self.lock()->m_editor->HandleQuit();
	}
}

void Application::ForceQuit()
{
	m_bLoop = false;
}

void Application::SetPlayMode(bool play)
{ 
	m_bPlayMode = play;
	
	//loop over components and do one more tick of update and render. This allows any components that need to stop themselves to do so.
	std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = m_registry->GetComponentRegistry();
	for (int i = 0; i < compRegistry->size(); i++)
	{
		compRegistry->at(i).second.m_comp->Update(compRegistry->at(i).second.m_bUseThreads, compRegistry->at(i).second.m_iMaxComponentsPerThread);
		compRegistry->at(i).second.m_comp->PreRender(compRegistry->at(i).second.m_bUseThreads, compRegistry->at(i).second.m_iMaxComponentsPerThread);
	}
}

void Application::BindEditor(Editor* editor)
{
	m_editor = editor;
}

void Application::RegisterCoreKeybinds()
{
	InputManager::AddKeybind(Keybind("Forward", { Input(SDLK_w, -1), Input(SDLK_UP, -1) }));
	InputManager::AddKeybind(Keybind("Back", { Input(SDLK_s, -1), Input(SDLK_DOWN, -1) }));
	InputManager::AddKeybind(Keybind("Left", { Input(SDLK_a, -1), Input(SDLK_LEFT, -1) }));
	InputManager::AddKeybind(Keybind("Right", { Input(SDLK_d, -1), Input(SDLK_RIGHT, -1) }));

	InputManager::AddKeybind(Keybind("LeftMouse", { Input(SDLK_UNKNOWN, 1) }));
	InputManager::AddKeybind(Keybind("MiddleMouse", { Input(SDLK_UNKNOWN, 2) }));
	InputManager::AddKeybind(Keybind("RightMouse", { Input(SDLK_UNKNOWN, 3) }));
}

void Application::LoadSettings()
{
	Logger::LogInformation("Loading game.ini settings");
	
	//m_gameRenderer->UpdateScreenSize(m_mainIniFile->GetIntSetting("Video", "ResolutionHeight", 1000), m_mainIniFile->GetIntSetting("Video", "ResolutionWidth", 2000)); //does this setting make sense? Currently this sets window size, but it should probably set draw size.
	m_gameRenderer->SetWindowFullScreen(m_mainIniFile->GetIntSetting("Video", "Fullscreen", 0));
	m_gameRenderer->SetVSyncMode(m_mainIniFile->GetIntSetting("Video", "VSync", 1));
	m_gameRenderer->SetRenderScale(m_mainIniFile->GetFloatSetting("Video", "RenderScale", 1.0f));
	m_gameRenderer->SetDrawImageFilter(m_mainIniFile->GetIntSetting("Video", "DrawImageFilter", 1)); //probably shouldnt be a setting, rather something that is setup in the editor and loaded from a scene file or something.

	m_audioManager->AddMixerOption("Master", m_mainIniFile->GetFloatSetting("Audio", "MasterVolume", 1.0f));
	m_audioManager->AddMixerOption("Ambience", m_mainIniFile->GetFloatSetting("Audio", "AmbientVolume", 1.0f));

	m_pStats->m_bLogPerformance = m_mainIniFile->GetBooleanSetting("Debug", "LogPerformance", false);
	m_logger->m_bUseLogging = m_mainIniFile->GetBooleanSetting("Debug", "UseLogging", false);
}

void Application::MainLoop()
{
	ImGuiIO& io = ImGui::GetIO();

	while (m_bLoop)
	{
		m_pStats->StartPerformanceMeasurement("Frame");
		m_pStats->StartPerformanceMeasurement("Pre-Update");

		std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = m_registry->GetComponentRegistry();
		InputManager::HandleGeneralInput();

		Uint32 windowFlags = SDL_GetWindowFlags(m_gameRenderer->GetWindow());
		if (windowFlags & SDL_WINDOW_MINIMIZED)
			continue;

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(m_gameRenderer->GetWindow());
		ImGui::NewFrame();

		m_pStats->EndPerformanceMeasurement("Pre-Update");

		//update start
		m_pStats->StartPerformanceMeasurement("Update");
		AudioManager::UpdateSystem();
		for (int i = 0; i < m_vToolUIs.size(); i++)
		{
			if (m_vToolUIs.at(i)->m_uiOpen)
			{
				m_vToolUIs.at(i)->DoInterface();
				m_vToolUIs.at(i)->DoModals();
			}
		}
		for (int i = 0; i < m_vEntities.size(); i++)
		{
			for (int o = 0; o < m_vEntities.at(i).m_vBehaviours.size(); o++)
			{
				m_vEntities.at(i).m_vBehaviours.at(o)->Update();
			}
		}
		for (int i = 0; i < compRegistry->size(); i++)
		{
			if (!m_bPlayMode && !compRegistry->at(i).second.m_bUpdateInEditor)
				continue;

			m_pStats->StartComponentMeasurement(compRegistry->at(i).first, true);

			compRegistry->at(i).second.m_comp->PreUpdate();
			compRegistry->at(i).second.m_comp->Update(compRegistry->at(i).second.m_bUseThreads, compRegistry->at(i).second.m_iMaxComponentsPerThread);

			m_pStats->EndComponentMeasurement(compRegistry->at(i).first, true);
		}
		ThreadingManager::WaitForTasksToClear();
		m_pStats->EndPerformanceMeasurement("Update");
		//update end

		if (m_editor != nullptr)
		{
			m_pStats->StartPerformanceMeasurement("EditorUpdate");
			m_editor->OnUpdate();
			m_pStats->EndPerformanceMeasurement("EditorUpdate");
		}

		//Render Start
		m_pStats->StartPerformanceMeasurement("Render");
		m_gameRenderer->UpdateScreenSize();
		m_gameRenderer->ResetForNextFrame();

		if (m_editor != nullptr)
		{
			m_pStats->StartPerformanceMeasurement("EditorRender");
			m_editor->OnRender();
			m_pStats->EndPerformanceMeasurement("EditorRender");
		}

		for (int i = 0; i < compRegistry->size(); i++)
		{
			if (!m_bPlayMode && !compRegistry->at(i).second.m_bRenderInEditor)
				continue;

			m_pStats->StartComponentMeasurement(compRegistry->at(i).first, false);

			compRegistry->at(i).second.m_comp->PreRender(compRegistry->at(i).second.m_bUseThreads, compRegistry->at(i).second.m_iMaxComponentsPerThread);

			m_pStats->EndComponentMeasurement(compRegistry->at(i).first, false);
		}
		ThreadingManager::WaitForTasksToClear();
		m_gameRenderer->DrawFrame();
		m_pStats->EndPerformanceMeasurement("Render");
		//Render End

		m_pStats->StartPerformanceMeasurement("Cleanup");
		CleanupDeletionEntities();
		InputManager::ClearFrameInputs();
		m_resourceManager->UnloadUnusedResources();
		m_pStats->EndPerformanceMeasurement("Cleanup");

		m_pStats->UpdatePerformanceStats();
		m_pStats->EndPerformanceMeasurement("Frame");
		m_pStats->LogPerformanceStats();
	}

	CleanupApplication();
}

void Application::CleanupApplication()
{
	Logger::LogInformation("Starting cleanup and closing engine!");

	m_gameRenderer->WaitForRenderingToFinish();
	m_gameRenderer->SetCamera(nullptr);

	ThreadingManager::StopThreads();
	ClearLoadedScene();

	m_resourceManager->UnloadAllResources();
	delete m_resourceManager;

	delete m_registry;
	delete m_gameRenderer;
	delete m_audioManager;
	delete m_threadManager;
	delete m_logger;
	delete m_pStats;
	delete m_sceneManager;

	SDL_Quit();
}

std::string Application::GetUniqueEntityID()
{
	std::string id = GenerateRandomString(25);
	while (GetEntityIndex(id) != -1)
	{
		id = GenerateRandomString(25);
	}

	return id;
}

Entity* Application::CreateEntity() //this will need optimisation
{
	if(!m_vDeletionEntities.empty())
	{
		Entity* targetEntity = m_vDeletionEntities.front();

		if (targetEntity->m_bAvailableForUse)
		{
			targetEntity->m_bAvailableForUse = false;
			m_vDeletionEntities.pop_front();
			return targetEntity;
		}
	}

	//generate entity ID here.
	std::string id = m_self.lock()->GetUniqueEntityID();
	m_vEntities.push_back(id);
	m_vEntities.at(m_vEntities.size() - 1).m_sEntityName = "New Entity";
	return &m_vEntities.at(m_vEntities.size() - 1);
}

Entity* Application::CreateEntity(std::string _desiredID, std::string _name, std::string _parentID)
{
	if (!m_vDeletionEntities.empty())
	{
		Entity* targetEntity = m_vDeletionEntities.front();

		if (targetEntity->m_bAvailableForUse && targetEntity->m_sEntityID == _desiredID)
		{
			targetEntity->m_bAvailableForUse = false;
			targetEntity->m_sEntityParentID = _parentID;
			m_vDeletionEntities.pop_front();
			return targetEntity;
		}
	}

	for (int i = 0; i < m_vEntities.size(); i++)
	{
		if (m_vEntities.at(i).m_sEntityID == _desiredID)
		{
			Logger::LogError(FormatString("Tried to create entity with duplicate ID. %s", _desiredID), 2);
			return nullptr;
		}
	}

	Entity newEnt(_desiredID);
	newEnt.m_sEntityParentID = _parentID;

	m_vEntities.push_back(newEnt);
	m_vEntities.at(m_vEntities.size() - 1).m_sEntityName = _name;
	return &m_vEntities.at(m_vEntities.size() - 1);
}

void Application::LinkChildEntities()
{
	for(int i = 0; i < m_vEntities.size(); i++)
	{
		if (m_vEntities.at(i).m_sEntityParentID == "")
			continue;

		int parentIndex = GetEntityIndex(m_vEntities.at(i).m_sEntityParentID);
		Entity* parentEntity = GetEntity(parentIndex);

		parentEntity->AddChildObject(m_vEntities.at(i).m_sEntityID);
	}
}

int Application::GetEntityIndex(std::string _ID)
{
	for (int i = 0; i < m_vEntities.size(); i++)
	{
		if (m_vEntities.at(i).m_sEntityID == _ID)
			return i;
	}

	return -1;
}

void Application::DeleteEntity(int index)
{
	if (index > m_vEntities.size() - 1 || index < 0)
		return;

	m_vDeletionEntities.push_back(&m_vEntities.at(index));
}

Entity* Application::GetEntity(int index)
{
	if (index > m_vEntities.size() - 1 || index < 0)
		return nullptr;

	return &m_vEntities.at(index);
}

void Application::ClearLoadedScene()
{
	for (int i = 0; i < m_vEntities.size(); i++)
	{
		m_vEntities.at(i).DeleteAllBehaviours();
	}

	m_vEntities.clear();
	m_vDeletionEntities.clear();

	m_gameRenderer->m_camera = nullptr;

	std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = m_registry->GetComponentRegistry();
	for (int i = 0; i < compRegistry->size(); i++)
	{
		compRegistry->at(i).second.m_comp->RemoveAllComponents();
	}
}

void Application::SetProjectFile(std::string path)
{
	std::shared_ptr<Application> app = m_self.lock();

	if (app->m_projectFile != nullptr && path == m_projectFile->GetProjectFilePath())
		return;

	if(app->m_projectFile != nullptr)
		delete m_projectFile;

	m_projectFile = new ProjectFile();
	m_projectFile->LoadProjectFile(path);

	if(m_gameDLL != nullptr)
		delete m_gameDLL;

	m_gameDLL = new NobleDLL(m_projectFile->m_sProjectDirectory + "\\NobleGame.dll");
	m_gameDLL->LoadDLL(GetApplication());

	if(app->m_editor != nullptr)
		app->m_editor->SetProjectFile(m_projectFile);
}

void Application::RegisterDefaultComponents()
{
	m_registry->RegisterComponent<Transform>("Transform", false, 1024, true, true);
	m_registry->RegisterComponent<Camera>("Camera", false, 1024, true, true);
	m_registry->RegisterComponent<AudioListener>("AudioListener", false, 1024, false, false);
	m_registry->RegisterComponent<AudioSource>("AudioSource", false, 1024, false, false);
	m_registry->RegisterComponent<MeshRenderer>("MeshRenderer", false, 1024, true, true);
	m_registry->RegisterComponent<ScriptEmbedder>("ScriptEmbedder", false, 1024, false, false);
	m_registry->RegisterComponent<Sprite>("Sprite", false, 1024, true, true);
	m_registry->RegisterComponent<Light>("Light", false, 1024, true, true);

	m_registry->RegisterBehaviour("DebugCam", new DebugCam());
}

void Application::ResetRegistries()
{
	m_registry->ClearRegistries();

	m_pStats->ClearComponentMeasurements();

	RegisterDefaultComponents();
	m_resourceManager->RegisterResourceTypes();
	m_gameRenderer->RegisterDescriptors();
}

void Application::CleanupDeletionEntities()
{
	while (!m_vDeletionEntities.empty())
	{
		Entity* currentEntity = m_vDeletionEntities.front();
		currentEntity->DeleteAllBehaviours();
		m_vDeletionEntities.pop_front();
		std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = m_registry->GetComponentRegistry();
		for (int i = 0; i < compRegistry->size(); i++)
		{
			compRegistry->at(i).second.m_comp->RemoveComponent(currentEntity->m_sEntityID);
		}
		currentEntity->m_bAvailableForUse = true;

		if (currentEntity->m_sEntityParentID != "")
		{
			int parentIndex = GetEntityIndex(currentEntity->m_sEntityParentID);
			GetEntity(parentIndex)->RemoveChildObject(currentEntity->m_sEntityID);
		}

		for (int i = 0; i < currentEntity->m_vChildEntityIDs.size(); i++)
		{
			int childIndex = GetEntityIndex(currentEntity->m_vChildEntityIDs.at(i));
			DeleteEntity(childIndex);
		}

		currentEntity->m_sEntityParentID = "";
		currentEntity->m_vChildEntityIDs.clear();
	}

	for (int i = 0; i < m_vEntities.size(); i++)
	{
		m_vEntities.at(i).GetAllComponents();
	}
}