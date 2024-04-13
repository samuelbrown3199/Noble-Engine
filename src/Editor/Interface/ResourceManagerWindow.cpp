#include "ResourceManagerWindow.h"

#include <Engine/Core/ResourceManager.h>
#include <Engine/Core/Registry.h>
#include <Engine/Core/EngineResources/AudioClip.h>
#include <Engine/Core/EngineResources/Texture.h>
#include <Engine/Core/EngineResources/Model.h>
#include <Engine/Core/EngineResources/Script.h>
#include <Engine/Core/ProjectFile.h>

void ResourceManagerWindow::InitializeInterface(ImGuiWindowFlags defaultFlags)
{
    EditorToolUI::InitializeInterface(defaultFlags);
    selResource = nullptr;
}

void ResourceManagerWindow::DoInterface()
{
    static int selectedRes = -1;
    static int selectedDefaultRes = -1;
    static int selectedShaderProg = -1;
    static Resource* defaultRes = nullptr;

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500.0f, 500.0f), ImGuiCond_Appearing);
    if (!ImGui::Begin("Resource Manager", &m_uiOpen, m_windowFlags))
    {
        ImGui::End();
        return;
    }

    std::map<int, std::pair<std::string, ResourceRegistry>>* resourceRegistry = NobleRegistry::GetResourceRegistry();

    for (int i = 0; i < resourceRegistry->size(); i++)
    {
        int res = 0;

        if (ImGui::Selectable(resourceRegistry->at(i).first.c_str(), selectedDefaultRes == res))
        {
            defaultRes = resourceRegistry->at(i).second.m_resource;
        }

        res++;
    }


    ImGui::SeparatorText("Resource Load Defaults");
    if (defaultRes != nullptr)
    {
        defaultRes->DoResourceInterface();

        if (ImGui::Button("Save Resource"))
        {
            Application::GetApplication()->GetProjectFile()->UpdateProjectFile();
            defaultRes->ReloadResource();
        }
    }

    ImGui::SeparatorText("Resources");

    if (ImGui::BeginMenu("Add Resource"))
    {
        for (int i = 0; i < resourceRegistry->size(); i++)
        {
            if (ImGui::MenuItem(resourceRegistry->at(i).first.c_str()))
            {
                if (resourceRegistry->at(i).second.m_bRequiresFile)
                {
                    std::string path = OpenFileSelectDialog(".mp3");
                    if (path != "")
                    {
                        resourceRegistry->at(i).second.m_resource->AddResource(path);
                    }
                }
                else
                {
                    m_bOpenNoFileResourceWindow = true;
                    m_iNewResourceType = i;
                }
            }
        }
        ImGui::EndMenu();
    }
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    for (int i = 0; i < resourceRegistry->size(); i++)
    {
        ImGui::Text(resourceRegistry->at(i).first.c_str());
        std::vector<std::shared_ptr<Resource>> resources = resourceRegistry->at(i).second.m_resource->GetResourcesOfType(); //Doing this every frame on a large database might hurt in future.

        for (int o = 0; o < resources.size(); o++)
        {
            if (ImGui::Selectable(resources.at(o)->m_sLocalPath.c_str(), selectedRes == o))
            {
                selResource = ResourceManager::GetResourceFromDatabase<Resource>(resources.at(o)->m_sLocalPath, resourceRegistry->at(i).second.m_bRequiresFile);
            }
        }
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SeparatorText("Resource Information");
    ImGui::BeginChild("Resource Info", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);

    if (selResource != nullptr) 
    {
        ResourceManager* resourceManager = Application::GetApplication()->GetResourceManager();

        selResource->DoResourceInterface();

        if (ImGui::Button("Save Resource"))
        {
            Application::GetApplication()->GetProjectFile()->UpdateProjectFile();
            selResource->ReloadResource();
        }
        ImGui::SameLine();
        if (ImGui::Button("Remove Resource"))
        {
            resourceManager->RemoveResourceFromDatabase(selResource->m_sLocalPath);
            Application::GetApplication()->GetProjectFile()->UpdateProjectFile();
            selResource = nullptr;
        }
    }
    ImGui::EndChild();

    ImGui::End();



    if (m_bOpenNoFileResourceWindow)
    {
        ImGui::OpenPopup("New Resource");
    }

    // Always center this window when appearing
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500.0f, 150.0f));
    if (ImGui::BeginPopupModal("New Resource"))
    {
        std::string newResourceName = "";
        ImGui::InputText("New Resource Name", &newResourceName);

        if (ImGui::Button("Create"))
        {
            resourceRegistry->at(m_iNewResourceType).second.m_resource->AddResource(newResourceName);
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