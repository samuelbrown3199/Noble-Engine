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

#include "ECS/Entity.h"

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
	rtn->m_logger->SetLogLevel("trace");
#endif

	LogInfo("Starting Engine, Version: " + GetVersionInfoString());

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

	rtn->m_gameRenderer = new Renderer(_windowName);
	rtn->m_resourceManager = new ResourceManager();
	rtn->m_audioManager = new AudioManager();
	rtn->m_threadManager = new ThreadingManager();
	rtn->m_inputManager = new InputManager();
	rtn->m_sceneManager = new SceneManager();

	rtn->RegisterDefaultComponents();

	rtn->RegisterCoreKeybinds();

	rtn->m_mainIniFile = std::make_shared<IniFile>(GetWorkingDirectory() + "\\game.ini");
	rtn->LoadSettings();

	std::vector<std::string> projectFiles = GetAllFilesOfType(GetWorkingDirectory(), ".npj", false);
	if (projectFiles.size() != 0)
	{
		rtn->SetProjectFile(projectFiles.at(0));
	}
	else
	{
		LogError("No project files found in working directory. Engine is probably running in editor mode.");
	}

	LogInfo("Engine started successfully");

	rtn->m_sceneManager->LoadDefaultScene();

	return rtn;
}

void Application::StopApplication()
{
	std::unique_lock<std::mutex> lock(m_criticalSection);

	if(m_self.lock()->m_editor == nullptr)
		m_bLoop = false;
	else
	{
		m_self.lock()->m_editor->HandleQuit();
	}
}

void Application::ForceQuit()
{
	std::unique_lock<std::mutex> lock(m_criticalSection);
	m_bLoop = false;
}

bool Application::GetMainLoop()
{
	std::unique_lock<std::mutex> lock(m_criticalSection);
	return m_bLoop;
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
	m_inputManager->AddKeybind(Keybind("Forward", { Input(SDLK_w, -1), Input(SDLK_UP, -1) }));
	m_inputManager->AddKeybind(Keybind("Back", { Input(SDLK_s, -1), Input(SDLK_DOWN, -1) }));
	m_inputManager->AddKeybind(Keybind("Left", { Input(SDLK_a, -1), Input(SDLK_LEFT, -1) }));
	m_inputManager->AddKeybind(Keybind("Right", { Input(SDLK_d, -1), Input(SDLK_RIGHT, -1) }));

	m_inputManager->AddKeybind(Keybind("LeftMouse", { Input(SDLK_UNKNOWN, 1) }));
	m_inputManager->AddKeybind(Keybind("MiddleMouse", { Input(SDLK_UNKNOWN, 2) }));
	m_inputManager->AddKeybind(Keybind("RightMouse", { Input(SDLK_UNKNOWN, 3) }));
}

void Application::LoadSettings()
{
	LogInfo("Loading game.ini settings");
	
	m_gameRenderer->UpdateScreenSize(m_mainIniFile->GetIntSetting("Video", "ResolutionHeight", 0), m_mainIniFile->GetIntSetting("Video", "ResolutionWidth", 0));
	m_gameRenderer->SetWindowFullScreen(m_mainIniFile->GetIntSetting("Video", "Fullscreen", 0));
	m_gameRenderer->SetVSyncMode(m_mainIniFile->GetIntSetting("Video", "VSync", 1));
	m_gameRenderer->SetRenderScale(m_mainIniFile->GetFloatSetting("Video", "RenderScale", 1.0f));

	m_audioManager->AddMixerOption("Master", m_mainIniFile->GetFloatSetting("Audio", "MasterVolume", 1.0f));
	m_audioManager->AddMixerOption("Ambience", m_mainIniFile->GetFloatSetting("Audio", "AmbientVolume", 1.0f));

	m_pStats->m_bLogPerformance = m_mainIniFile->GetBooleanSetting("Debug", "LogPerformance", false);
	m_logger->SetLogLevel(m_mainIniFile->GetStringSetting("Debug", "LogLevel", "info"));
}

void Application::MainLoop()
{
	ImGuiIO& io = ImGui::GetIO();

	while (m_bLoop)
	{
		m_pStats->StartPerformanceMeasurement("Frame");
		m_pStats->StartPerformanceMeasurement("Pre-Update");

		std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = m_registry->GetComponentRegistry();
		m_inputManager->HandleGeneralInput();

		Uint32 windowFlags = SDL_GetWindowFlags(m_gameRenderer->GetWindow());
		if (windowFlags & SDL_WINDOW_MINIMIZED)
			continue;

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(m_gameRenderer->GetWindow());
		ImGui::NewFrame();

		m_pStats->EndPerformanceMeasurement("Pre-Update");

		//update start
		m_pStats->StartPerformanceMeasurement("Update");
		m_audioManager->UpdateSystem();
		for (int i = 0; i < m_vToolUIs.size(); i++)
		{
			if (m_vToolUIs.at(i)->m_uiOpen)
			{
				m_vToolUIs.at(i)->DoInterface();
				m_vToolUIs.at(i)->DoModals();
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
		m_threadManager->WaitForTasksToClear();
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
		m_gameRenderer->CheckScreenSizeForUpdate();
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
		m_threadManager->WaitForTasksToClear();
		m_gameRenderer->DrawFrame();
		m_pStats->EndPerformanceMeasurement("Render");
		//Render End

		m_pStats->StartPerformanceMeasurement("Cleanup");
		CleanupDeletionEntities();
		m_inputManager->ClearFrameInputs();
		m_resourceManager->UnloadUnusedResources();
		m_pStats->EndPerformanceMeasurement("Cleanup");

		m_pStats->UpdatePerformanceStats();
		m_pStats->UpdateMemoryUsageStats();
		m_pStats->EndPerformanceMeasurement("Frame");
		m_pStats->LogPerformanceStats();
	}

	CleanupApplication();
}

void Application::CleanupApplication()
{
	LogInfo("Starting cleanup and closing engine!");

	m_gameRenderer->WaitForRenderingToFinish();
	m_gameRenderer->SetCamera(nullptr);

	if(m_editor != nullptr)
		m_editor->CleanupEditor();

	m_threadManager->StopThreads();
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
	while (m_vEntities.count(id) != 0)
	{
		id = GenerateRandomString(25);
	}

	return id;
}

Entity* Application::CreateEntity()
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
	std::string id = GetUniqueEntityID();
	m_vEntities[id] = Entity(id);
	m_vEntities[id].m_sEntityName = "New Entity";
	return &m_vEntities.at(id);
}

Entity* Application::CreateEntity(std::string _desiredID, std::string _name, std::string _parentID)
{
	if (_desiredID == "")
	{
		_desiredID = GetUniqueEntityID();
	}

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

	if(m_vEntities.count(_desiredID) != 0)
	{
		Entity* targetEntity = &m_vEntities.at(_desiredID);
		if (targetEntity->m_sEntityID == _desiredID && targetEntity->m_bAvailableForUse)
		{
			targetEntity->m_bAvailableForUse = false;
			targetEntity->m_sEntityParentID = _parentID;
			targetEntity->m_sEntityName = _name;
			return targetEntity;
		}
		else if (targetEntity->m_sEntityID == _desiredID && !targetEntity->m_bAvailableForUse)
		{
			LogFatalError("Entity with ID " + _desiredID + " already exists and is in use. Generating new ID.");
			return nullptr;
		}
	}

	Entity newEnt(_desiredID);
	newEnt.m_sEntityParentID = _parentID;

	m_vEntities[_desiredID] = newEnt;
	m_vEntities[_desiredID].m_sEntityName = _name;
	return &m_vEntities.at(_desiredID);
}

void Application::LinkChildEntities()
{
	std::unordered_map<std::string, Entity>::iterator it;
	for (it = m_vEntities.begin(); it != m_vEntities.end(); it++)
	{
		if (it->second.m_sEntityParentID == "")
			continue;

		Entity* parentEntity = GetEntity(it->second.m_sEntityParentID);
		parentEntity->AddChildObject(m_vEntities.at(it->first).m_sEntityID);
	}
}

void Application::DeleteEntity(std::string ID)
{
	m_vDeletionEntities.push_back(&m_vEntities.at(ID));
}

Entity* Application::GetEntity(std::string ID)
{
	if(ID == "")
		return nullptr;

	return &m_vEntities.at(ID);
}

void Application::ClearLoadedScene()
{
	m_vEntities.clear();
	m_vDeletionEntities.clear();

	m_gameRenderer->RemoveCamera();

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
}

void Application::ResetRegistries()
{
	m_registry->ClearRegistries();

	m_pStats->ClearComponentMeasurements();

	RegisterDefaultComponents();
	m_resourceManager->RegisterResourceTypes();
	m_gameRenderer->RegisterDescriptors();
}

void Application::PushCommand(Command* command)
{
	m_editor->PushCommand(command);
}

void Application::ConsoleLog(std::string message)
{
	if(m_editor != nullptr)
		m_editor->ConsoleLog(message);
}

void Application::CleanupDeletionEntities()
{
	while (!m_vDeletionEntities.empty())
	{
		Entity* currentEntity = m_vDeletionEntities.front();
		m_vDeletionEntities.pop_front();
		std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = m_registry->GetComponentRegistry();
		for (int i = 0; i < compRegistry->size(); i++)
		{
			compRegistry->at(i).second.m_comp->RemoveComponent(currentEntity->m_sEntityID);
		}
		currentEntity->m_bAvailableForUse = true;

		if (currentEntity->m_sEntityParentID != "")
		{
			GetEntity(currentEntity->m_sEntityParentID)->RemoveChildObject(currentEntity->m_sEntityID);
		}

		for (int i = 0; i < currentEntity->m_vChildEntityIDs.size(); i++)
		{
			DeleteEntity(currentEntity->m_vChildEntityIDs.at(i));
		}

		currentEntity->m_sEntityParentID = "";
		currentEntity->m_vChildEntityIDs.clear();
	}

	std::unordered_map<std::string, Entity>::iterator it;
	for (it = m_vEntities.begin(); it != m_vEntities.end(); it++)
	{
		it->second.GetAllComponents();
	}
}