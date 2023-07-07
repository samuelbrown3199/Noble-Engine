#include <Engine/Core/Application.h>
#include <Engine/Useful.h>
#include <Engine\Behaviours\DebugCam.h>
#include <Engine/ECS/Entity.hpp>
#include <Engine/Systems/MeshRenderer.h>

#include <Engine/Core/ResourceManager.h>
#include <Engine/Resource/Model.h>
#include <Engine/Resource/Texture.h>

#include "EditorUI.h"

#ifndef NDEBUG
int main()
{
    std::shared_ptr<Application> app = Application::StartApplication("Noble Editor Debug");
    //app->BindBehaviour<DebugCam>();

    std::shared_ptr<EditorUI> ui = Application::BindDebugUI<EditorUI>();
    ui->m_uiOpen = true;

    app->MainLoop();
    return 0;
}
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    std::shared_ptr<Application> app = Application::StartApplication("Noble Editor");
    //app->BindBehaviour<DebugCam>();

    /*int amount = 10;
    for (int x = 0; x < amount; x++)
    {
        for (int y = 0; y < amount; y++)
        {
            for (int z = 0; z < amount; z++)
            {
                Entity* entity = Application::CreateEntity();
                Transform* tr = entity->AddComponent<Transform>();

                tr->m_position = glm::vec3(x, y, z);

                MeshRenderer* mr = entity->AddComponent<MeshRenderer>();
                mr->m_model = ResourceManager::LoadResource<Model>("GameData\\Models\\cube.obj");
                mr->m_texture = ResourceManager::LoadResource<Texture>("GameData\\Textures\\cottage_diffuse.png");
                mr->m_colour = glm::vec4(1, 1, 1, 1);
            }
        }
    }*/

    std::shared_ptr<EditorUI> ui = Application::BindDebugUI<EditorUI>();
    ui->m_uiOpen = true;

    app->MainLoop();
    return 0;
}
#endif