#include "ResourceManagerWindow.h"

#include <Engine/Core/ResourceManager.h>
#include <Engine/Core/Registry.h>
#include <Engine/Core/EngineResources/AudioClip.h>
#include <Engine/Core/EngineResources/Texture.h>
#include <Engine/Core/EngineResources/Model.h>
#include <Engine/Core/EngineResources/Script.h>
#include <Engine/Core/ProjectFile.h>

#include "../EditorManagement/EditorManager.h"
#include "DataEditorWindow.h"
#include "SceneHierarchyWindow.h"

void ResourceManagerWindow::InitializeInterface(ImGuiWindowFlags defaultFlags)
{
    EditorToolUI::InitializeInterface(defaultFlags);
    selResource = nullptr;
}

void ResourceManagerWindow::DoInterface()
{
    static int selectedRes = -1;
    static int selectedDefaultRes = -1;
    static Resource* defaultRes = nullptr;

    EditorManager* editorManager = dynamic_cast<EditorManager*>(m_pEditor);
    ResourceManager* rManager = Application::GetApplication()->GetResourceManager();

    NobleRegistry* registry = Application::GetApplication()->GetRegistry();
    std::vector<std::pair<std::string, ResourceRegistryBase*>>* resourceRegistry = registry->GetResourceRegistry();

    if (!ImGui::Begin("Resource Manager", &m_uiOpen, m_windowFlags))
    {
        ImGui::End();
        return;
    }

    UpdateWindowState();

    if (ImGui::BeginMenuBar())
    {
		if (ImGui::BeginMenu("File"))
		{
            if (ImGui::BeginMenu("Import Resource"))
            {
                for (int i = 0; i < resourceRegistry->size(); i++)
                {
                    if (!resourceRegistry->at(i).second->m_bRequiresFile)
                        continue;

                    if (ImGui::MenuItem(resourceRegistry->at(i).first.c_str()))
                    {
                        IGFD::FileDialogConfig config;
                        ImGuiFileDialog::Instance()->OpenDialog("ChooseResource", "Choose Resource", resourceRegistry->at(i).second->m_sAcceptedFileTypes.c_str(), config);
                        m_iNewResourceType = i;
                    }
                }

                ImGui::EndMenu();
			}
            if (ImGui::BeginMenu("Create New Resource"))
            {
                for (int i = 0; i < resourceRegistry->size(); i++)
                {
                    if (resourceRegistry->at(i).second->m_bRequiresFile)
                        continue;

                    if (ImGui::MenuItem(resourceRegistry->at(i).first.c_str()))
                    {
                        m_bOpenNoFileResourceWindow = true;
                        m_iNewResourceType = i;
                    }
                }

                ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

    if (ImGuiFileDialog::Instance()->Display("ChooseResource"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            std::string copyDir = GetGameDataFolder() + "\\" + fileName;
            CopyFileToDestination(filePath + "\\" + fileName, copyDir);

            resourceRegistry->at(m_iNewResourceType).second->m_resource->AddResource(copyDir);
            Application::GetApplication()->GetProjectFile()->UpdateProjectFile();
        }
        ImGuiFileDialog::Instance()->Close();
    }

    for (int i = 0; i < resourceRegistry->size(); i++)
    {
        int res = 0;

        if (ImGui::Selectable(resourceRegistry->at(i).first.c_str(), selectedDefaultRes == res))
        {
            defaultRes = resourceRegistry->at(i).second->m_resource;
        }

        res++;
    }


    ImGui::SeparatorText("Resource Load Defaults");
    if (defaultRes != nullptr)
    {
        defaultRes->DoResourceInterface();

        if (editorManager->m_projectFile == nullptr)
        {
            ImGui::BeginDisabled();
        }
        if (ImGui::Button("Save Resource"))
        {
            Application::GetApplication()->GetProjectFile()->UpdateProjectFile();
            defaultRes->ReloadResource();
        }
        if (editorManager->m_projectFile == nullptr)
        {
            ImGui::EndDisabled();
        }
    }

    ImGui::SeparatorText("Resources");

    for (int i = 0; i < resourceRegistry->size(); i++)
    {
        ImGui::Text(resourceRegistry->at(i).first.c_str());
        std::vector<std::shared_ptr<Resource>> resources = rManager->GetAllResourcesOfType(resourceRegistry->at(i).first); //Doing this every frame on a large database might hurt in future.

        for (int o = 0; o < resources.size(); o++)
        {
            if (ImGui::Selectable(resources.at(o)->m_sLocalPath.c_str(), selectedRes == o))
            {
                ResourceManager* resourceManager = Application::GetApplication()->GetResourceManager();
                selResource = resourceManager->GetResourceFromDatabase<Resource>(resources.at(o)->m_sLocalPath, resourceRegistry->at(i).second->m_bRequiresFile);

                dynamic_cast<SceneHierarchyWindow*>(editorManager->GetEditorUI("SceneHierarchy"))->ResetSelectedEntity();
                dynamic_cast<DataEditorWindow*>(editorManager->GetEditorUI("DataEditor"))->SetSelectedResource(selResource);
            }
        }
    }

    ImGui::End();

    if (m_bOpenNoFileResourceWindow)
    {
        ImGui::OpenPopup("New Resource");
    }

    if (ImGui::BeginPopupModal("New Resource"))
    {
        std::string newResourceName = "";
        ImGui::InputText("New Resource Name", &newResourceName);

        if (ImGui::Button("Create"))
        {
            resourceRegistry->at(m_iNewResourceType).second->m_resource->AddResource(newResourceName);
            m_bOpenNoFileResourceWindow = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            m_bOpenNoFileResourceWindow = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}