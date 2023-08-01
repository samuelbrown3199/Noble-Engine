#include "ResourceManagerWindow.h"

#include <Engine/Resource/AudioClip.h>
#include <Engine/Resource/Texture.h>
#include <Engine/Resource/Model.h>

void ResourceManagerWindow::InitializeInterface()
{
    selResource = nullptr;
}

void ResourceManagerWindow::DoInterface()
{
    static int selectedAC = -1;
    static int selectedTex = -1;
    static int selectedMod = -1;

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500.0f, 500.0f), ImGuiCond_Appearing);
    if (!ImGui::Begin("Resource Manager", &m_uiOpen))
    {
        ImGui::End();
        return;
    }

    //Need to have a think on this at some point.

    ImGui::Text("Audio Clips");
    int ac = 0;
    nlohmann::json audioClips = ResourceManager::m_resourceDatabaseJson.at("AudioClip");
    for (auto it : audioClips.items())
    {
        if (ImGui::Selectable(it.key().c_str(), selectedAC == ac))
        {
            selectedAC = ac;
            selectedTex = -1;
            selectedMod = -1;

            selResource = ResourceManager::GetResourceFromDatabase<AudioClip>(it.key());
        }

        ac++;
    }

    ImGui::Text("Textures");
    int tex = 0;
    nlohmann::json textures = ResourceManager::m_resourceDatabaseJson.at("Texture");
    for (auto it : textures.items())
    {
        if (ImGui::Selectable(it.key().c_str(), selectedTex == tex))
        {
            selectedTex = tex;
            selectedAC = -1;
            selectedMod = -1;

            selResource = ResourceManager::GetResourceFromDatabase<Texture>(it.key());
        }

        tex++;
    }

    ImGui::Text("Models");
    int mod = 0;
    nlohmann::json models = ResourceManager::m_resourceDatabaseJson.at("Model");
    for (auto it : models.items())
    {
        if (ImGui::Selectable(it.key().c_str(), selectedMod == mod))
        {
            selectedMod = mod;
            selectedAC = -1;
            selectedTex = -1;

            selResource = ResourceManager::GetResourceFromDatabase<Model>(it.key());
        }

        mod++;
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SeparatorText("Resource Information");
    ImGui::BeginChild("Resource Info", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);

    if (selResource != nullptr) 
    {
        if (selectedAC != -1)
            AudioClip::DoResourceInterface(selResource);
        else if (selectedTex != -1)
            Texture::DoResourceInterface(selResource);
        else if (selectedMod != -1)
            Model::DoResourceInterface(selResource);

        if (ImGui::Button("Save Resource"))
        {
            ResourceManager::WriteResourceDatabase();
            selResource->ReloadResource();
        }
    }
    ImGui::EndChild();

    ImGui::End();
}