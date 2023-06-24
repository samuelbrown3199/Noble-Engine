#include "Application.h"
#include "InputManager.h"

#include "../Systems/AudioSourceSystem.h"
#include "../Systems/SpriteSystem.h"
#include "../Systems/TransformSystem.h"
#include "../Systems/MeshRendererSystem.h"
#include "../Behaviours/DebugCam.h"

#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../imgui/backends/imgui_impl_opengl3.h"

bool Application::m_bLoop = true;
std::weak_ptr<Application> Application::m_self;

std::deque<Entity*> Application::m_vDeletionEntities;
std::vector<Entity> Application::m_vEntities;

std::vector<std::shared_ptr<UISystem>> Application::m_vUiSystems;
std::vector<std::shared_ptr<Behaviour>> Application::m_vBehaviours;
std::vector<std::shared_ptr<SystemBase>> Application::m_vComponentSystems;
std::vector<std::shared_ptr<DebugUI>> Application::m_vDebugUIs;

std::shared_ptr<ShaderProgram> Application::m_mainShaderProgram;
std::shared_ptr<ShaderProgram> Application::m_uiShaderProgram;
std::shared_ptr<ShaderProgram> Application::m_uiTextProgram;

//----------------- Private Functions ----------------------


//----------------- Public Functions -----------------------

std::shared_ptr<Application> Application::StartApplication(const std::string _windowName)
{
	srand(time(NULL));

	std::shared_ptr<Application> rtn = std::make_shared<Application>();
	rtn->m_logger = new Logger();
	rtn->m_gameRenderer = new Renderer(_windowName);
	rtn->m_audioManager = new AudioManager();
	rtn->m_threadManager = new ThreadingManager();
	rtn->m_resourceManager = new ResourceManager();
	rtn->m_pStats = new PerformanceStats();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(Renderer::GetWindow(), Renderer::GetGLContext());
	ImGui_ImplOpenGL3_Init(rtn->m_gameRenderer->GetGLSLVersion());

	rtn->BindDebugUI<EditorUI>()->m_uiOpen = true;

	rtn->m_mainIniFile = ResourceManager::LoadResource<IniFile>("game.ini");
	rtn->m_mainShaderProgram = ResourceManager::CreateShaderProgram();
	rtn->m_mainShaderProgram->BindShader(ResourceManager::LoadResource<Shader>("GameData\\Shaders\\standard.vs"), GL_VERTEX_SHADER);
	rtn->m_mainShaderProgram->BindShader(ResourceManager::LoadResource<Shader>("GameData\\Shaders\\standard.fs"), GL_FRAGMENT_SHADER);
	rtn->m_mainShaderProgram->LinkShaderProgram(rtn->m_mainShaderProgram);

	rtn->m_uiShaderProgram = ResourceManager::CreateShaderProgram();
	rtn->m_uiShaderProgram->BindShader(ResourceManager::LoadResource<Shader>("GameData\\Shaders\\standardUI.vs"), GL_VERTEX_SHADER);
	rtn->m_uiShaderProgram->BindShader(ResourceManager::LoadResource<Shader>("GameData\\Shaders\\standardUI.fs"), GL_FRAGMENT_SHADER);
	rtn->m_uiShaderProgram->LinkShaderProgram(rtn->m_uiShaderProgram);

	rtn->m_uiTextProgram = ResourceManager::CreateShaderProgram();
	rtn->m_uiTextProgram->BindShader(ResourceManager::LoadResource<Shader>("GameData\\Shaders\\standardText.vs"), GL_VERTEX_SHADER);
	rtn->m_uiTextProgram->BindShader(ResourceManager::LoadResource<Shader>("GameData\\Shaders\\standardText.fs"), GL_FRAGMENT_SHADER);
	rtn->m_uiTextProgram->LinkShaderProgram(rtn->m_uiTextProgram);

	UIQuads::SetupUIQuads();

	rtn->BindSystem<TransformSystem>(SystemUsage::useUpdate, "Transform");
	rtn->BindSystem<AudioSourceSystem>(SystemUsage::useUpdate, "AudioSource");
	rtn->BindSystem<SpriteSystem>(SystemUsage::useRender, "Sprite");
	rtn->BindSystem<MeshRendererSystem>(SystemUsage::useRender, "Mesh");

	rtn->BindBehaviour<DebugCam>();

	Logger::LogInformation("Engine started successfully");

	rtn->LoadSettings();
	rtn->m_self = rtn;
	return rtn;
}

void Application::LoadSettings()
{
	Logger::LogInformation("Loading game.ini settings");
	
	m_gameRenderer->UpdateScreenSize(m_mainIniFile->GetIntSetting("Video", "ResolutionHeight", 500), m_mainIniFile->GetIntSetting("Video", "ResolutionWidth", 500));
	m_gameRenderer->SetWindowFullScreen(m_mainIniFile->GetIntSetting("Video", "Fullscreen", 0));
	m_gameRenderer->SetVSyncMode(m_mainIniFile->GetIntSetting("Video", "VSync", 1));
	m_gameRenderer->SetFov(m_mainIniFile->GetFloatSetting("Video", "FOV", 90.0f));

	m_audioManager->AddMixerOption("master", m_mainIniFile->GetFloatSetting("Audio", "MasterVolume", 1.0f));
	m_audioManager->AddMixerOption("ambience", m_mainIniFile->GetFloatSetting("Audio", "AmbientVolume", 1.0f));

	m_pStats->printPerformance = m_mainIniFile->GetBooleanSetting("Debug", "PrintPerformance", false);
	m_logger->m_bUseLogging = m_mainIniFile->GetBooleanSetting("Debug", "Log", false);
}

void Application::MainLoop()
{
	Camera* cam = Renderer::GetCamera();

	ImGuiIO& io = ImGui::GetIO();

	while (m_bLoop)
	{
		m_pStats->ResetPerformanceStats();
		m_pStats->preUpdateStart = SDL_GetTicks();
		InputManager::HandleGeneralInput();
		m_pStats->preUpdateTime = SDL_GetTicks() - m_pStats->preUpdateStart;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		//update start
		m_pStats->updateStart = SDL_GetTicks();
		AudioManager::UpdateListenerPos();
		for (int i = 0; i < m_vComponentSystems.size(); i++)
		{
			Uint32 updateStart = SDL_GetTicks();
			m_vComponentSystems.at(i)->PreUpdate();
			m_vComponentSystems.at(i)->Update();
			Uint32 updateEnd = SDL_GetTicks() - updateStart;

			std::pair<std::string, Uint32> pair(m_vComponentSystems.at(i)->m_systemID, updateEnd);
			m_pStats->m_mSystemUpdateTimes.push_back(pair);
		}
		for (int i = 0; i < m_vBehaviours.size(); i++)
		{
			m_vBehaviours.at(i)->Update();
		}
		for (int i = 0; i < m_vUiSystems.size(); i++)
		{
			if (m_vUiSystems.at(i)->m_bActive)
			{
				m_vUiSystems.at(i)->Update();
				m_vUiSystems.at(i)->HandleEvents();
			}
		}
		ThreadingManager::WaitForTasksToClear();
		m_pStats->updateTime = SDL_GetTicks() - m_pStats->updateStart;
		//update end

		//Render Start
		m_pStats->renderStart = SDL_GetTicks();

		for (int i = 0; i < m_vDebugUIs.size(); i++)
		{
			if (m_vDebugUIs.at(i)->m_uiOpen)
			{
				m_vDebugUIs.at(i)->DoInterface();
			}
		}

		ImGui::Render();
		m_gameRenderer->UpdateScreenSize();
		m_gameRenderer->ClearBuffer();
		for (int i = 0; i < m_vComponentSystems.size(); i++)
		{
			Uint32 renderStart = SDL_GetTicks();
			m_vComponentSystems.at(i)->PreRender();
			m_vComponentSystems.at(i)->Render();
			Uint32 renderEnd = SDL_GetTicks() - renderStart;

			std::pair<std::string, Uint32> pair(m_vComponentSystems.at(i)->m_systemID, renderEnd);
			m_pStats->m_mSystemRenderTimes.push_back(pair);
		}
		for (int i = 0; i < m_vUiSystems.size(); i++)
		{
			if (m_vUiSystems.at(i)->m_bActive)
			{
				m_vUiSystems.at(i)->Render();
			}
		}
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		ThreadingManager::WaitForTasksToClear();
		m_gameRenderer->SwapGraphicsBuffer();
		m_pStats->renderTime = SDL_GetTicks() - m_pStats->renderStart;
		//Render End

		m_pStats->cleanupStart = SDL_GetTicks();
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

	ThreadingManager::StopThreads();
	delete m_gameRenderer;
	delete m_audioManager;
	delete m_networkManager;
	delete m_threadManager;
	delete m_resourceManager;
	delete m_logger;
	delete m_pStats;
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
	return &m_vEntities.at(m_vEntities.size() - 1);
}

Entity* Application::CreateEntity(std::string _desiredID)
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
	return &m_vEntities.at(m_vEntities.size() - 1);
}

void Application::DeleteEntity(std::string _ID)
{
	for(int i = 0; i < m_vEntities.size(); i++)
	{
		if(m_vEntities.at(i).m_sEntityID == _ID)
			m_vDeletionEntities.push_back(&m_vEntities.at(i));
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
	m_vEntities.clear();
	m_vDeletionEntities.clear();

	for (int i = 0; i < m_vComponentSystems.size(); i++)
	{
		m_vComponentSystems.at(i)->RemoveAllComponents();
	}
}

void Application::CleanupDeletionEntities()
{
	while (!m_vDeletionEntities.empty())
	{
		Entity* currentEntity = m_vDeletionEntities.front();
		for (int o = 0; o < m_vComponentSystems.size(); o++)
		{
			m_vComponentSystems.at(o)->RemoveComponent(currentEntity->m_sEntityID);
		}
		currentEntity->m_bAvailableForUse = true;
	}
}

std::shared_ptr<SystemBase> Application::GetSystemFromID(std::string _ID)
{
	for (int i = 0; i < m_vComponentSystems.size(); i++)
	{
		if (m_vComponentSystems.at(i)->m_systemID == _ID)
		{
			return m_vComponentSystems.at(i);
		}
	}

	return nullptr;
}

void Application::CreateNetworkManager(const int& _mode)
{
	if (!m_self.lock()->m_networkManager)
	{
		m_self.lock()->m_networkManager = new NetworkManager(_mode);
		std::thread* networkThread = new std::thread([=] {
			m_self.lock()->m_networkManager->m_parser->Loop();
			});
		networkThread->detach();
	}
}