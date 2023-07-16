#include "Application.h"
#include "InputManager.h"

#include "../Systems/AudioListenerSystem.h"
#include "../Systems/AudioSourceSystem.h"
#include "../Systems/CameraSystem.h"
#include "../Systems/SpriteSystem.h"
#include "../Systems/TransformSystem.h"
#include "../Systems/MeshRendererSystem.h"

#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../imgui/backends/imgui_impl_vulkan.h"

bool Application::m_bEntitiesDeleted = false;
bool Application::m_bLoop = true;
std::weak_ptr<Application> Application::m_self;

std::deque<Entity*> Application::m_vDeletionEntities;
std::vector<Entity> Application::m_vEntities;

std::vector<std::shared_ptr<SystemBase>> Application::m_vComponentSystems;
std::vector<std::shared_ptr<DebugUI>> Application::m_vDebugUIs;

//----------------- Private Functions ----------------------


//----------------- Public Functions -----------------------

std::shared_ptr<Application> Application::StartApplication(const std::string _windowName)
{
	srand(time(NULL));

	std::shared_ptr<Application> rtn = std::make_shared<Application>();
	rtn->m_logger = new Logger();

#ifndef NDEBUG
	//We want to log initialization at least in debug mode.
	rtn->m_logger->m_bUseLogging = true;
#endif

	rtn->m_resourceManager = new ResourceManager();
	rtn->m_gameRenderer = new Renderer(_windowName);
	rtn->m_audioManager = new AudioManager();
	rtn->m_threadManager = new ThreadingManager();
	rtn->m_pStats = new PerformanceStats();

	rtn->InitializeImGui();

	rtn->m_mainIniFile = ResourceManager::LoadResource<IniFile>("game.ini");

	rtn->BindSystem<TransformSystem>(SystemUsage::useUpdate, "Transform");
	rtn->BindSystem<CameraSystem>(SystemUsage::useUpdate, "Camera");
	rtn->BindSystem<AudioListenerSystem>(SystemUsage::useUpdate, "AudioListener");
	rtn->BindSystem<AudioSourceSystem>(SystemUsage::useUpdate, "AudioSource");
	rtn->BindSystem<SpriteSystem>(SystemUsage::useRender, "Sprite");
	rtn->BindSystem<MeshRendererSystem>(SystemUsage::useRender, "Mesh");

	rtn->LoadSettings();
	rtn->m_self = rtn;

	Logger::LogInformation("Engine started successfully");

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
	//ImGuiIO& io = ImGui::GetIO();

	while (m_bLoop)
	{
		m_pStats->ResetPerformanceStats();
		m_pStats->preUpdateStart = SDL_GetTicks();
		InputManager::HandleGeneralInput();
		m_pStats->preUpdateTime = SDL_GetTicks() - m_pStats->preUpdateStart;

		//update start
		m_pStats->updateStart = SDL_GetTicks();
		AudioManager::UpdateSystem();
		for (int i = 0; i < m_vComponentSystems.size(); i++)
		{
			Uint32 updateStart = SDL_GetTicks();
			m_vComponentSystems.at(i)->PreUpdate();
			m_vComponentSystems.at(i)->Update();
			Uint32 updateEnd = SDL_GetTicks() - updateStart;

			std::pair<std::string, Uint32> pair(m_vComponentSystems.at(i)->m_systemID, updateEnd);
			m_pStats->m_mSystemUpdateTimes.push_back(pair);
		}
		for (int i = 0; i < m_vEntities.size(); i++)
		{
			for (int o = 0; o < m_vEntities.at(i).m_vBehaviours.size(); o++)
			{
				m_vEntities.at(i).m_vBehaviours.at(o)->Update();
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

		//ImGui::Render();
		m_gameRenderer->UpdateScreenSize();
		m_gameRenderer->StartDrawFrame();
		for (int i = 0; i < m_vComponentSystems.size(); i++)
		{
			Uint32 renderStart = SDL_GetTicks();
			m_vComponentSystems.at(i)->PreRender();
			m_vComponentSystems.at(i)->Render();
			Uint32 renderEnd = SDL_GetTicks() - renderStart;

			std::pair<std::string, Uint32> pair(m_vComponentSystems.at(i)->m_systemID, renderEnd);
			m_pStats->m_mSystemRenderTimes.push_back(pair);
		}
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		ThreadingManager::WaitForTasksToClear();
		m_gameRenderer->EndDrawFrame();
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

	vkDeviceWaitIdle(Renderer::GetLogicalDevice());
	CleanupApplication();
}

void Application::CleanupApplication()
{
	Logger::LogInformation("Starting cleanup and closing engine!");

	ThreadingManager::StopThreads();

	ClearLoadedScene();

	m_resourceManager->UnloadAllResources();
	delete m_resourceManager;

	delete m_gameRenderer;
	delete m_audioManager;
	delete m_networkManager;
	delete m_threadManager;
	delete m_logger;
	delete m_pStats;

	SDL_Quit();
}

void Application::InitializeImGui()
{
	//Not quite in a position to use this yet.

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	/*ImGui_ImplSDL2_InitForVulkan(Renderer::GetWindow());
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = Renderer::GetVulkanInstance();
	init_info.PhysicalDevice = Renderer::GetPhysicalDevice();
	init_info.Device = Renderer::GetLogicalDevice();
	init_info.QueueFamily = YOUR_QUEUE_FAMILY;
	init_info.Queue = YOUR_QUEUE;
	init_info.PipelineCache = YOUR_PIPELINE_CACHE;
	init_info.DescriptorPool = YOUR_DESCRIPTOR_POOL;
	init_info.Subpass = 0;
	init_info.MinImageCount = 2;
	init_info.ImageCount = 2;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Allocator = YOUR_ALLOCATOR;
	init_info.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&init_info, Renderer::GetGraphicsPipeline()->GetRenderPass());
	// (this gets a bit more complicated, see example app for full reference)
	ImGui_ImplVulkan_CreateFontsTexture(Renderer::GetCurrentCommandBuffer());
	// (your code submit a queue)
	ImGui_ImplVulkan_DestroyFontUploadObjects();*/
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
		m_vDeletionEntities.pop_front();
		for (int o = 0; o < m_vComponentSystems.size(); o++)
		{
			m_vComponentSystems.at(o)->RemoveComponent(currentEntity->m_sEntityID);
		}
		currentEntity->m_bAvailableForUse = true;

		m_bEntitiesDeleted = true;
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