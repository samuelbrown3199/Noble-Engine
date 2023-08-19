#include "ResourceManagerWindow.h"

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
    for (int i = 0; i < resourceRegistry->size(); i++)
    {
        ImGui::Text(resourceRegistry->at(i).first.c_str());
        std::vector<std::shared_ptr<Resource>> resources = resourceRegistry->at(i).second->GetResourcesOfType(); //Doing this every frame on a large database might hurt in future.

        int res = 0;

        for (int i = 0; i < resources.size(); i++)
        {
            if (ImGui::Selectable(resources.at(i)->m_sLocalPath.c_str(), selectedRes == i))
            {
                selResource = ResourceManager::GetResourceFromDatabase<Resource>(resources.at(i)->m_sLocalPath);
            }

            res++;
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
    }
    ImGui::EndChild();

    ImGui::End();
}