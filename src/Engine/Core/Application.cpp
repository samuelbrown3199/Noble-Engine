#include "Application.h"
#include "InputManager.h"
#include "Registry.h"

#include "../Components/Transform.h"
#include "../Components/AudioListener.h"
#include "../Components/AudioSource.h"
#include "../Components/Camera.h"
#include "../Components/MeshRenderer.h"
#include "../Components/ScriptEmbedder.h"
#include "../Components/Sprite.h"

#include "../ECS/Entity.hpp"

#include "../Behaviours/DebugCam.h"

#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../imgui/backends/imgui_impl_opengl3.h"

bool Application::m_bEntitiesDeleted = false;
bool Application::m_bLoop = true;
std::weak_ptr<Application> Application::m_self;
bool Application::m_bPlayMode = true;

std::deque<Entity*> Application::m_vDeletionEntities;
std::vector<Entity> Application::m_vEntities;

std::vector<std::shared_ptr<DebugUI>> Application::m_vDebugUIs;

//----------------- Private Functions ----------------------


//----------------- Public Functions -----------------------

std::shared_ptr<Application> Application::StartApplication(const std::string _windowName)
{
	srand(time(NULL));

	std::shared_ptr<Application> rtn = std::make_shared<Application>();

	rtn->m_registry = new NobleRegistry();
	rtn->m_logger = new Logger();

#ifndef NDEBUG
	//We want to log initialization at least in debug mode.
	rtn->m_logger->m_bUseLogging = true;
#endif

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

	rtn->m_gameRenderer = new Renderer(_windowName);
	rtn->m_resourceManager = new ResourceManager();
	rtn->m_audioManager = new AudioManager();
	rtn->m_threadManager = new ThreadingManager();
	rtn->m_pStats = new PerformanceStats();

	rtn->m_registry->RegisterComponent("Transform", new Transform(), false, 1024, true, true);
	rtn->m_registry->RegisterComponent("Camera", new Camera(), false, 1024, true, true);
	rtn->m_registry->RegisterComponent("AudioListener", new AudioListener(), false, 1024, false, false);
	rtn->m_registry->RegisterComponent("AudioSource", new AudioSource(), false, 1024, false, false);
	rtn->m_registry->RegisterComponent("MeshRenderer", new MeshRenderer(), false, 1024, true, true);
	rtn->m_registry->RegisterComponent("ScriptEmbedder", new ScriptEmbedder(), false, 1024, false, false);
	rtn->m_registry->RegisterComponent("Sprite", new Sprite(), false, 1024, true, true);

	rtn->m_registry->RegisterBehaviour("DebugCam", new DebugCam());

	rtn->RegisterCoreKeybinds();
	rtn->InitializeImGui();

	rtn->m_mainIniFile = std::make_shared<IniFile>(GetWorkingDirectory() + "\\game.ini");

	rtn->LoadSettings();
	rtn->m_self = rtn;

	Logger::LogInformation("Engine started successfully");

	Renderer::SetClearColour(glm::vec3(0.0f, 0.25, 0.75));

	return rtn;
}

void Application::SetPlayMode(bool play)
{ 
	m_bPlayMode = play;
	
	//loop over components and do one more tick of update and render. This allows any components that need to stop themselves
	std::map<int, std::pair<std::string, ComponentRegistry>>* compRegistry = NobleRegistry::GetComponentRegistry();
	for (int i = 0; i < compRegistry->size(); i++)
	{
		compRegistry->at(i).second.m_comp->Update(compRegistry->at(i).second.m_bUseThreads, compRegistry->at(i).second.m_iMaxComponentsPerThread);
		compRegistry->at(i).second.m_comp->Render(compRegistry->at(i).second.m_bUseThreads, compRegistry->at(i).second.m_iMaxComponentsPerThread);
	}
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
	
	m_gameRenderer->UpdateScreenSize(m_mainIniFile->GetIntSetting("Video", "ResolutionHeight", 1000), m_mainIniFile->GetIntSetting("Video", "ResolutionWidth", 2000));
	m_gameRenderer->SetWindowFullScreen(m_mainIniFile->GetIntSetting("Video", "Fullscreen", 0));
	m_gameRenderer->SetVSyncMode(m_mainIniFile->GetIntSetting("Video", "VSync", 1));
	m_gameRenderer->SetFov(m_mainIniFile->GetFloatSetting("Video", "FOV", 90.0f));

	m_audioManager->AddMixerOption("master", m_mainIniFile->GetFloatSetting("Audio", "MasterVolume", 1.0f));
	m_audioManager->AddMixerOption("ambience", m_mainIniFile->GetFloatSetting("Audio", "AmbientVolume", 1.0f));

	m_pStats->printPerformance = m_mainIniFile->GetBooleanSetting("Debug", "PrintPerformance", false);
	m_logger->m_bUseLogging = m_mainIniFile->GetBooleanSetting("Debug", "UseLogging", false);
}

void Application::MainLoop()
{
	ImGuiIO& io = ImGui::GetIO();

	while (m_bLoop)
	{
		std::map<int, std::pair<std::string, ComponentRegistry>>* compRegistry = m_registry->GetComponentRegistry();

		m_pStats->ResetPerformanceStats();
		m_pStats->preUpdateStart = SDL_GetTicks();
		InputManager::HandleGeneralInput();

		Uint32 windowFlags = SDL_GetWindowFlags(m_gameRenderer->GetWindow());
		if (windowFlags & SDL_WINDOW_MINIMIZED)
			continue;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(Renderer::GetWindow());

		ImGui::NewFrame();

		m_pStats->preUpdateTime = SDL_GetTicks() - m_pStats->preUpdateStart;

		//update start
		m_pStats->updateStart = SDL_GetTicks();

		AudioManager::UpdateSystem();

		for (int i = 0; i < m_vDebugUIs.size(); i++)
		{
			if (m_vDebugUIs.at(i)->m_uiOpen)
			{
				m_vDebugUIs.at(i)->DoInterface();
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

			Uint32 updateStart = SDL_GetTicks();
			compRegistry->at(i).second.m_comp->PreUpdate();
			compRegistry->at(i).second.m_comp->Update(compRegistry->at(i).second.m_bUseThreads, compRegistry->at(i).second.m_iMaxComponentsPerThread);
			Uint32 updateEnd = SDL_GetTicks() - updateStart;

			std::pair<std::string, Uint32> pair(compRegistry->at(i).first, updateEnd);
			m_pStats->m_mSystemUpdateTimes.push_back(pair);
		}
		ThreadingManager::WaitForTasksToClear();
		m_pStats->updateTime = SDL_GetTicks() - m_pStats->updateStart;
		//update end

		//Render Start
		m_pStats->renderStart = SDL_GetTicks();
		ImGui::Render();
		m_gameRenderer->UpdateScreenSize();
		m_gameRenderer->StartFrameRender();
		for (int i = 0; i < compRegistry->size(); i++)
		{
			if (!m_bPlayMode && !compRegistry->at(i).second.m_bRenderInEditor)
				continue;

			Uint32 renderStart = SDL_GetTicks();
			compRegistry->at(i).second.m_comp->PreRender();
			compRegistry->at(i).second.m_comp->Render(compRegistry->at(i).second.m_bUseThreads, compRegistry->at(i).second.m_iMaxComponentsPerThread);
			Uint32 renderEnd = SDL_GetTicks() - renderStart;

			std::pair<std::string, Uint32> pair(compRegistry->at(i).first, renderEnd);
			m_pStats->m_mSystemRenderTimes.push_back(pair);
		}
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		ThreadingManager::WaitForTasksToClear();
		m_gameRenderer->EndFrameRender();
		m_pStats->renderTime = SDL_GetTicks() - m_pStats->renderStart;
		//Render End

		m_pStats->cleanupStart = SDL_GetTicks();
		m_bEntitiesDeleted = false;
		CleanupDeletionEntities();
		InputManager::ClearFrameInputs();
		ResourceManager::UnloadUnusedResources();
		m_pStats->cleanupTime = SDL_GetTicks() - m_pStats->cleanupStart;

		m_pStats->UpdatePerformanceStats();
		m_pStats->PrintOutPerformanceStats();
		m_pStats->m_mSystemUpdateTimes.clear();
		m_pStats->m_mSystemRenderTimes.clear();
	}

	CleanupApplication();
}

void Application::CleanupApplication()
{
	Logger::LogInformation("Starting cleanup and closing engine!");

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

	SDL_Quit();
}

void Application::InitializeImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(Renderer::GetWindow(), Renderer::GetGLContext());
	ImGui_ImplOpenGL3_Init(m_gameRenderer->GetGLSLVersion());
}

std::string Application::GetUniqueEntityID()
{
	std::string id = GenerateRandomString(25);
	while (GetEntity(id) != nullptr)
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

Entity* Application::CreateEntity(std::string _desiredID, std::string _name)
{
	if (!m_vDeletionEntities.empty())
	{
		Entity* targetEntity = m_vDeletionEntities.front();

		if (targetEntity->m_bAvailableForUse && targetEntity->m_sEntityID == _desiredID)
		{
			targetEntity->m_bAvailableForUse = false;
			m_vDeletionEntities.pop_front();
			return targetEntity;
		}
	}

	for (int i = 0; i < m_vEntities.size(); i++)
	{
		if (m_vEntities.at(i).m_sEntityID == _desiredID)
		{
			return nullptr;
		}
	}

	m_vEntities.push_back(_desiredID);
	m_vEntities.at(m_vEntities.size() - 1).m_sEntityName = _name;
	return &m_vEntities.at(m_vEntities.size() - 1);
}

void Application::DeleteEntity(std::string _ID)
{
	for(int i = 0; i < m_vEntities.size(); i++)
	{
		if (m_vEntities.at(i).m_sEntityID == _ID)
		{
			m_vDeletionEntities.push_back(&m_vEntities.at(i));
			break;
		}
	}
}

Entity* Application::GetEntity(std::string _ID)
{
	for (int i = 0; i < m_vEntities.size(); i++)
	{
		if (m_vEntities.at(i).m_sEntityID == _ID)
			return &m_vEntities.at(i);
	}
	return nullptr;
}

void Application::ClearLoadedScene()
{
	for (int i = 0; i < m_vEntities.size(); i++)
	{
		m_vEntities.at(i).DeleteAllBehaviours();
	}

	m_vEntities.clear();
	m_vDeletionEntities.clear();

	std::map<int, std::pair<std::string, ComponentRegistry>>* compRegistry = NobleRegistry::GetComponentRegistry();
	for (int i = 0; i < compRegistry->size(); i++)
	{
		compRegistry->at(i).second.m_comp->RemoveAllComponents();
	}
}

void Application::CleanupDeletionEntities()
{
	while (!m_vDeletionEntities.empty())
	{
		Entity* currentEntity = m_vDeletionEntities.front();
		currentEntity->DeleteAllBehaviours();
		m_vDeletionEntities.pop_front();
		std::map<int, std::pair<std::string, ComponentRegistry>>* compRegistry = NobleRegistry::GetComponentRegistry();
		for (int i = 0; i < compRegistry->size(); i++)
		{
			compRegistry->at(i).second.m_comp->RemoveComponent(currentEntity->m_sEntityID);
		}
		currentEntity->m_bAvailableForUse = true;

		m_bEntitiesDeleted = true;
	}

	for (int i = 0; i < m_vEntities.size(); i++)
	{
		m_vEntities.at(i).GetAllComponents();
	}
}