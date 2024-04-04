#include "EditorUI.h"

#include <Engine\Useful.h>
#include <Engine\Core\Application.h>
#include <Engine\Core\Graphics\Renderer.h>
#include <Engine\Core\SceneManager.h>
#include <Engine\Core\EngineBehaviours\DebugCam.h>
#include <Engine\Core\InputManager.h>

#include <Engine\Core\ECS\Entity.hpp>

#include <Engine\Core\EngineResources\AudioClip.h>
#include <Engine\Core\EngineResources\Model.h>
#include <Engine\Core\EngineResources\Texture.h>
#include <Engine\Core\EngineResources\Script.h>

#include "../EditorManagement/ProjectFile.h"

int EditorUI::m_iSelEntity = -1;
int EditorUI::m_iSelSystem = -1;

Entity* EditorUI::m_DebugCam = nullptr;

void EditorUI::ChangeEditorMode()
{
	Application::SetPlayMode(!Application::GetPlayMode());

	if(Application::GetPlayMode() == false)
		LoadScene(SceneManager::GetCurrentSceneLocalPath());
}

void EditorUI::CreateEditorCam()
{
	if (!m_DebugCam)
	{
		m_DebugCam = Application::CreateEntity();
		m_DebugCam->m_sEntityName = "Editor Cam";
		m_DebugCam->AddBehaviour<DebugCam>();
	}
	else
	{
		Application::DeleteEntity(Application::GetEntityIndex(m_DebugCam->m_sEntityID));
		m_DebugCam = nullptr;
	}
}

void EditorUI::UpdateEditorWindowTitle()
{
	std::string title = m_sWindowName;
	if (m_projectFile)
		title += "\t(Project " + m_projectFile->m_sProjectName + ")";
	
	title += "\t(" + SceneManager::GetCurrentSceneLocalPath() + ")";
	Renderer::UpdateWindowTitle(title);
}

void EditorUI::OpenResourceManager()
{
	if (!m_projectFile)
		return;

	m_resourceManagerWind->m_uiOpen = !m_resourceManagerWind->m_uiOpen;
}

void EditorUI::OpenProfiler()
{
	m_profilerWind->m_uiOpen = !m_profilerWind->m_uiOpen;
}

void EditorUI::UpdateLightingWidgets()
{
	clearColour = ImVec4(Renderer::GetClearColour().x, Renderer::GetClearColour().y, Renderer::GetClearColour().z, 200.0f / 255.0f);
}

void EditorUI::LoadScene(std::string scenePath)
{
	SceneManager::LoadScene(scenePath);

	UpdateEditorWindowTitle();
	UpdateLightingWidgets();
}

void EditorUI::InitializeInterface()
{
	m_windowFlags |= ImGuiWindowFlags_NoMove;
	m_windowFlags |= ImGuiWindowFlags_NoTitleBar;
	m_windowFlags |= ImGuiWindowFlags_NoResize;

	m_sWindowName = Renderer::GetWindowTitle();

	m_resourceManagerWind = Application::BindDebugUI<ResourceManagerWindow>();
	m_profilerWind = Application::BindDebugUI<Profiler>();

	UpdateLightingWidgets();
}

void EditorUI::DoInterface()
{
	HandleShortcutInputs();

	DoMainMenuBar();

	ImGui::SetNextWindowSize(ImVec2(450, Renderer::GetScreenSize().y));

	ImGui::Begin("Editor", &m_uiOpen, m_windowFlags);

	std::string playModeButton = "Enter Play Mode";
	if(Application::GetPlayMode())
		playModeButton = "Enter Edit Mode";
	if (ImGui::Button(playModeButton.c_str()))
	{
		ChangeEditorMode();
	}

	std::vector<Entity>& entities = Application::GetEntityList();
	if(ImGui::Button("Create Entity"))
	{
		Application::CreateEntity();
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Entity"))
	{
		if(m_iSelEntity != -1)
			Application::DeleteEntity(Application::GetEntityIndex(entities.at(m_iSelEntity).m_sEntityID));
	}

	ImGui::SameLine();
	if (ImGui::Button("Add Child Entity"))
	{
		if (m_iSelEntity != -1)
			entities.at(m_iSelEntity).CreateChildObject();
	}

	if (ImGui::TreeNode("Entities"))
	{
		std::map<int, std::pair<std::string, ComponentRegistry>>* compRegistry = NobleRegistry::GetComponentRegistry();
		std::map<int, std::pair<std::string, Behaviour*>>* behaviourRegistry = NobleRegistry::GetBehaviourRegistry();

		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		static int selection_mask = (1 << 2);
		for (int i = 0; i < entities.size(); i++)
			entities.at(i).DoEntityInterface(compRegistry, behaviourRegistry, i, m_iSelEntity);

		ImGui::TreePop();
	}

	Renderer* renderer = Application::GetRenderer();

	ImGui::Dummy(ImVec2(0.0f, 20.0f));

	ImGui::ColorEdit3("Clear Colour", (float*)&clearColour);
	Renderer::SetClearColour(glm::vec3(clearColour.x, clearColour.y, clearColour.z));

	ambientColour = ImVec4(renderer->m_sceneData.ambientColour.x, renderer->m_sceneData.ambientColour.y, renderer->m_sceneData.ambientColour.z, renderer->m_sceneData.ambientColour.w);
	ImGui::ColorEdit4("Ambient Colour", (float*)&ambientColour);
	renderer->m_sceneData.ambientColour = glm::vec4(ambientColour.x, ambientColour.y, ambientColour.z, ambientColour.w);

	sunlightColour = ImVec4(renderer->m_sceneData.sunlightColour.x, renderer->m_sceneData.sunlightColour.y, renderer->m_sceneData.sunlightColour.z, renderer->m_sceneData.sunlightColour.w);
	ImGui::ColorEdit3("Sunlight Colour", (float*)&sunlightColour);
	renderer->m_sceneData.sunlightColour = glm::vec4(sunlightColour.x, sunlightColour.y, sunlightColour.z, 1.0f);

	float sunStrength = renderer->m_sceneData.sunlightDirection.w;
	ImGui::DragFloat("Sun Strength", &sunStrength, .1f, 0.0f, 1.0f);

	float sunDir[3] = { renderer->m_sceneData.sunlightDirection.x, renderer->m_sceneData.sunlightDirection.y, renderer->m_sceneData.sunlightDirection.z };
	ImGui::DragFloat3("Sunlight Direction", sunDir, 0.01f);
	renderer->m_sceneData.sunlightDirection = glm::vec4(sunDir[0], sunDir[1], sunDir[2], sunStrength);

	static float renderScale = renderer->GetRenderScale();
	ImGui::DragFloat("Render Scale", &renderScale, 0.1f, 0.1f, 1.0f, "%.2f");
	renderer->SetRenderScale(renderScale);

	ImGui::End();

	if (show_demo_window)
	{
		ImGui::ShowDemoWindow();
		ImPlot::ShowDemoWindow();
	}
}


void EditorUI::HandleShortcutInputs()
{
	if (InputManager::GetKey(SDLK_LCTRL) && InputManager::GetKeyDown(SDLK_q))
		CreateEditorCam();

	if (InputManager::GetKey(SDLK_LCTRL) && InputManager::GetKeyDown(SDLK_r))
		OpenResourceManager();

	if (InputManager::GetKey(SDLK_LCTRL) && InputManager::GetKeyDown(SDLK_p))
		OpenProfiler();

	if (InputManager::GetKey(SDLK_LCTRL) && InputManager::GetKeyDown(SDLK_RETURN))
		ChangeEditorMode();

	if (InputManager::GetKey(SDLK_LCTRL) && InputManager::GetKeyDown(SDLK_s))
		SceneManager::SaveLoadedScene();
}


void EditorUI::DoMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			DoFileMenu();
			ImGui::EndMenu();
		}
		DoNewProjectModal();

		if (m_projectFile)
		{
			if (ImGui::BeginMenu("Resources"))
			{
				DoAssetMenu();
				ImGui::EndMenu();
			}
		}

		if (ImGui::BeginMenu("Tools"))
		{
			DoToolMenu();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debug"))
		{
			DoDebugMenu();
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void EditorUI::DoFileMenu()
{
	ImGui::MenuItem("Project", NULL, false, false);
	if (ImGui::MenuItem("New Project"))
	{
		m_bOpenNewProj = true;
	}
	if (ImGui::BeginMenu("Load Project"))
	{
		std::vector<std::string> projects = GetAllFilesOfType(GetWorkingDirectory() + "\\Projects", ".npj");
		for (int n = 0; n < projects.size(); n++)
		{
			if (ImGui::Button(projects.at(n).c_str()))
			{
				if (m_projectFile == nullptr)
					m_projectFile = new ProjectFile();

				m_projectFile->LoadProjectFile(projects.at(n));
				UpdateEditorWindowTitle();
			}
		}

		ImGui::EndMenu();
	}

	ImGui::MenuItem("Scene", NULL, false, false);
	if (m_projectFile)
	{
		if (ImGui::MenuItem("New Scene"))
		{
			m_selComponent = nullptr;
			SceneManager::ClearLoadedScene();
		}
		if (ImGui::BeginMenu("Open Scene"))
		{
			std::string path = GetGameDataFolder();
			std::vector<std::string> scenes = GetAllFilesOfType(path, ".nsc");
			for (int n = 0; n < scenes.size(); n++)
			{
				if (ImGui::Button(GetFolderLocationRelativeToGameData(scenes.at(n)).c_str()))
				{
					LoadScene(GetFolderLocationRelativeToGameData(scenes.at(n)));
					UpdateEditorWindowTitle();
					UpdateLightingWidgets();
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Save Scene", "(CTRL+S)"))
		{
			if (Application::GetPlayMode())
			{
				return;
			}

			SceneManager::SaveLoadedScene();
		}

		if (ImGui::BeginMenu("Save Scene As..."))
		{
			if (Application::GetPlayMode())
			{
				return;
			}

			std::string newSceneName;
			ImGui::InputText("Scene Name", &newSceneName);

			static std::string path = "";
			if (!newSceneName.empty())
			{
				path = ResourceManager::GetResourceManagerWorkingDirectory() + "\\GameData\\" + newSceneName + ".nsc";
			}

			ImGui::SameLine();
			if (ImGui::Button("Create"))
			{
				SceneManager::SaveScene(path);
			}

			ImGui::EndMenu();
		}
	}
	else
	{
		ImGui::MenuItem("New Scene", NULL, false, false);
		ImGui::BeginMenu("Open Scene", false);
		ImGui::MenuItem("Save Scene", NULL, false, false);
		ImGui::MenuItem("Save Scene As...", NULL, false, false);
	}

	ImGui::MenuItem("Settings", NULL, false, false);
	if (ImGui::MenuItem("Quit Editor"))
	{
		Application::StopApplication();
	}
}

void EditorUI::DoAssetMenu()
{
	ImGui::MenuItem("Resources", NULL, false, false);
	if (ImGui::BeginMenu("Add Resource"))
	{
		std::map<int, std::pair<std::string, ResourceRegistry>>* resourceRegistry = NobleRegistry::GetResourceRegistry();

		for (int i = 0; i < resourceRegistry->size(); i++)
		{
			if (ImGui::MenuItem(resourceRegistry->at(i).first.c_str()))
			{
				std::string path = OpenFileSelectDialog(".mp3");
				if (path != "")
				{
					resourceRegistry->at(i).second.m_resource->AddResource(path);
				}
			}
		}
		ImGui::EndMenu();
	}

	if (ImGui::MenuItem("Resource Manager", "(CTRL+R)"))
	{
		OpenResourceManager();
	}
}

void EditorUI::DoToolMenu()
{
	ImGui::MenuItem("Tools", NULL, false, false);

	std::string camButton = "Create Editor Cam";
	if(m_DebugCam != nullptr)
		camButton = "Delete Editor Cam";
	if (ImGui::MenuItem(camButton.c_str(), "(Ctrl+Q)"))
	{
		CreateEditorCam();
	}

	if (ImGui::MenuItem("Profiler", "(CTRL+P)"))
	{
		OpenProfiler();
	}
}

void EditorUI::DoDebugMenu()
{
	ImGui::Checkbox("Demo Window", &show_demo_window);
}

void EditorUI::DoNewProjectModal()
{
	if (m_bOpenNewProj)
	{
		ImGui::OpenPopup("New Project");
	}

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(500.0f, 150.0f));
	if (ImGui::BeginPopupModal("New Project"))
	{
		static char buf1[64] = ""; ImGui::InputText("Project Name", buf1, 64);
		static char buf2[256] = ""; ImGui::InputText("Project Directory", buf2, 256);

		if (ImGui::Button("Create"))
		{
			ProjectFile::CreateProjectFile(buf1, buf2);

			if (m_projectFile == nullptr)
				m_projectFile = new ProjectFile();

			m_projectFile->LoadProjectFile(GetWorkingDirectory() + "\\Projects\\" + buf1 + ".npj");
			UpdateEditorWindowTitle();

			m_bOpenNewProj = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			m_bOpenNewProj = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}