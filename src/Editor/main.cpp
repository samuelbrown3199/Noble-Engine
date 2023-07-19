#include <Engine/Core/Application.h>
#include <Engine/Useful.h>
#include <Engine\Behaviours\DebugCam.h>
#include <Engine/ECS/Entity.hpp>
#include <Engine/Systems/MeshRenderer.h>

#include <Engine/Core/ResourceManager.h>
#include <Engine/Resource/Model.h>
#include <Engine/Resource/Texture.h>

#include "EditorUI.h"

void StartupEditor()
{
    std::shared_ptr<Application> app = Application::StartApplication("Noble Editor Debug");

    std::shared_ptr<EditorUI> ui = Application::BindDebugUI<EditorUI>();
    ui->m_uiOpen = true;

    Entity* entity = Application::CreateEntity();
    entity->AddBehaviour<DebugCam>();

    for (int i = 0; i < 2000; i++)
    {
        entity = Application::CreateEntity();
        entity->AddComponent<Transform>(glm::vec3(i, i, i));
        MeshRenderer* mr = entity->AddComponent<MeshRenderer>();

        if (i % 2 == 0)
        {
            mr->m_model = ResourceManager::LoadResource<Model>("GameData\\Models\\viking_room.obj");
            mr->m_texture = ResourceManager::LoadResource<Texture>("GameData\\Textures\\viking_room.png");
        }
        else
        {
            mr->m_model = ResourceManager::LoadResource<Model>("GameData\\Models\\cottage_obj.obj");
            mr->m_texture = ResourceManager::LoadResource<Texture>("GameData\\Textures\\cottage_diffuse.png");
        }
    }
    app->MainLoop();
}

#ifndef NDEBUG
int main()
{
    StartupEditor();
    return 0;
}
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    StartupEditor();
    return 0;
}
#endif