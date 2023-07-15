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

    //std::shared_ptr<EditorUI> ui = Application::BindDebugUI<EditorUI>();
    //ui->m_uiOpen = true;

    Entity* entity = Application::CreateEntity();
    entity->AddBehaviour<DebugCam>();

    app->MainLoop();
    return 0;
}
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    std::shared_ptr<Application> app = Application::StartApplication("Noble Editor");
    //app->BindBehaviour<DebugCam>();

    Entity* entity = Application::CreateEntity();
    entity->AddBehaviour<DebugCam>();

    //std::shared_ptr<EditorUI> ui = Application::BindDebugUI<EditorUI>();
    //ui->m_uiOpen = true;

    app->MainLoop();
    return 0;
}
#endif