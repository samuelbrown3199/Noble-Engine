#include "ResourceManagerWindow.h"

#include <Engine/Core/ResourceManager.h>
#include <Engine/Core/Registry.h>
#include <Engine/Resource/AudioClip.h>
#include <Engine/Resource/Texture.h>
#include <Engine/Resource/Model.h>
#include <Engine/Resource/Script.h>

void ResourceManagerWindow::InitializeInterface()
{
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
    if (!ImGui::Begin("Resource Manager", &m_uiOpen))
    {
        ImGui::End();
        return;
    }

    std::map<int, std::pair<std::string, Resource*>>* resourceRegistry = NobleRegistry::GetResourceRegistry();

    for (int i = 0; i < resourceRegistry->size(); i++)
    {
        int res = 0;

        if (ImGui::Selectable(resourceRegistry->at(i).first.c_str(), selectedDefaultRes == res))
        {
            defaultRes = resourceRegistry->at(i).second;
        }

        res++;
    }


    ImGui::SeparatorText("Resource Load Defaults");
    if (defaultRes != nullptr)
    {
        defaultRes->DoResourceInterface();

        if (ImGui::Button("Save Resource"))
        {
            ResourceManager::WriteResourceDatabase();
            defaultRes->ReloadResource();
        }
    }

    ImGui::SeparatorText("Resources");

    if (ImGui::BeginMenu("Add Resource"))
    {
        std::map<int, std::pair<std::string, Resource*>>* resourceRegistry = NobleRegistry::GetResourceRegistry();

        for (int i = 0; i < resourceRegistry->size(); i++)
        {
            if (ImGui::MenuItem(resourceRegistry->at(i).first.c_str()))
            {
                std::string path = OpenFileSelectDialog(".mp3");
                if (path != "")
                {
                    resourceRegistry->at(i).second->AddResource(path);
                }
            }
        }
        ImGui::EndMenu();
    }
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    for (int i = 0; i < resourceRegistry->size(); i++)
    {
        ImGui::Text(resourceRegistry->at(i).first.c_str());
        std::vector<std::shared_ptr<Resource>> resources = resourceRegistry->at(i).second->GetResourcesOfType(); //Doing this every frame on a large database might hurt in future.

        if (resourceRegistry->at(i).second->m_resourceType == "Shader") //temp hack
            continue;

        for (int o = 0; o < resources.size(); o++)
        {
            if (ImGui::Selectable(resources.at(o)->m_sLocalPath.c_str(), selectedRes == o))
            {
                selResource = ResourceManager::GetResourceFromDatabase<Resource>(resources.at(o)->m_sLocalPath);
            }
        }
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SeparatorText("Resource Information");
    ImGui::BeginChild("Resource Info", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);

    if (selResource != nullptr) 
    {
        selResource->DoResourceInterface();

        if (ImGui::Button("Save Resource"))
        {
            ResourceManager::WriteResourceDatabase();
            selResource->ReloadResource();
        }
        ImGui::SameLine();
        if (ImGui::Button("Remove Resource"))
        {
            ResourceManager::RemoveResourceFromDatabase(selResource->m_sLocalPath);
            selResource = nullptr;
        }
    }
    ImGui::EndChild();

    ImGui::SeparatorText("Shader Programs");
    std::vector<std::shared_ptr<ShaderProgram>>* shaderPrograms = ResourceManager::GetShaderPrograms();

    for (int o = 0; o < shaderPrograms->size(); o++)
    {
        if (ImGui::Selectable(shaderPrograms->at(o)->m_shaderProgramID.c_str(), selectedShaderProg == o))
        {
            selectedShaderProg = o;
        }
    }

    if (selectedShaderProg != -1)
    {  
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        shaderPrograms->at(selectedShaderProg)->DoShaderProgramInterface();
    }

    ImGui::End();
}