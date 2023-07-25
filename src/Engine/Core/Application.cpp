#include "Application.h"
#include "InputManager.h"

#include "../Systems/AudioListenerSystem.h"
#include "../Systems/AudioSourceSystem.h"
#include "../Systems/CameraSystem.h"
#include "../Systems/TransformSystem.h"
#include "../Systems/MeshRendererSystem.h"
#include "../Systems/SpriteSystem.h"

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

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

	rtn->m_resourceManager = new ResourceManager();
	rtn->m_gameRenderer = new Renderer(_windowName);
	rtn->m_audioManager = new AudioManager();
	rtn->m_threadManager = new ThreadingManager();
	rtn->m_pStats = new PerformanceStats();

	rtn->RegisterCoreKeybinds();

	rtn->InitializeImGui();

	rtn->m_mainIniFile = ResourceManager::LoadResource<IniFile>("game.ini");

	rtn->BindSystem<TransformSystem>(SystemUsage::useUpdate, "Transform");
	rtn->BindSystem<CameraSystem>(SystemUsage::useUpdate, "Camera");
	rtn->BindSystem<AudioListenerSystem>(SystemUsage::useUpdate, "AudioListener");
	rtn->BindSystem<AudioSourceSystem>(SystemUsage::useUpdate, "AudioSource");
	rtn->BindSystem<MeshRendererSystem>(SystemUsage::useRender, "Mesh");
	rtn->BindSystem<SpriteSystem>(SystemUsage::useRender, "Sprite");

	rtn->LoadSettings();
	rtn->m_self = rtn;

	Logger::LogInformation("Engine started successfully");

	return rtn;
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
		m_pStats->ResetPerformanceStats();
		m_pStats->preUpdateStart = SDL_GetTicks();
		InputManager::HandleGeneralInput();
		m_pStats->preUpdateTime = SDL_GetTicks() - m_pStats->preUpdateStart;

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(Renderer::GetWindow());

		ImGui::NewFrame();

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

	vkDestroyDescriptorPool(Renderer::GetLogicalDevice(), m_imguiPool, nullptr);
	ImGui_ImplVulkan_Shutdown();

	ThreadingManager::StopThreads();

	ClearLoadedScene();

	for (int i = 0; i < m_vComponentSystems.size(); i++)
	{
		m_vComponentSystems.at(i).reset();
	}

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
	//1: create descriptor pool for IMGUI
	// the size of the pool is very oversize, but it's copied from imgui demo itself.
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	vkCreateDescriptorPool(Renderer::GetLogicalDevice(), &pool_info, nullptr, &m_imguiPool);


	// 2: initialize imgui library

	//this initializes the core structures of imgui
	ImGui::CreateContext();

	//this initializes imgui for SDL
	ImGui_ImplSDL2_InitForVulkan(Renderer::GetWindow());

	//this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = Renderer::GetVulkanInstance();
	init_info.PhysicalDevice = Renderer::GetPhysicalDevice();
	init_info.Device = Renderer::GetLogicalDevice();
	init_info.Queue = Renderer::GetGraphicsQueue();
	init_info.DescriptorPool = m_imguiPool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.MSAASamples = Renderer::GetMSAALevel();

	ImGui_ImplVulkan_Init(&init_info, Renderer::GetGraphicsPipeline()->GetRenderPass());

	//execute a gpu command to upload imgui font textures
	VkCommandBuffer fontBuffer = Renderer::BeginSingleTimeCommand();
		ImGui_ImplVulkan_CreateFontsTexture(fontBuffer);
	Renderer::EndSingleTimeCommands(fontBuffer);

	//clear font textures from cpu data
	ImGui_ImplVulkan_DestroyFontUploadObjects();
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
		currentEntity->DeleteAllBehaviours();
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