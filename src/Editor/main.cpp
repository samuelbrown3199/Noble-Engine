#include <Engine/Core/Application.h>
#include <Engine/Useful.h>
#include <Engine\Behaviours\DebugCam.h>

#include "EditorUI.h"

#ifndef NDEBUG
int main()
{
    std::shared_ptr<Application> app = Application::StartApplication("Noble Editor Debug");
    app->BindBehaviour<DebugCam>();

    std::shared_ptr<EditorUI> ui = Application::BindDebugUI<EditorUI>();
    ui->m_uiOpen = true;

    app->MainLoop();
    return 0;
}
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    std::shared_ptr<Application> app = Application::StartApplication("Noble Editor");
    app->BindBehaviour<DebugCam>();

    std::shared_ptr<EditorUI> ui = Application::BindDebugUI<EditorUI>();
    ui->m_uiOpen = true;

    app->MainLoop();
    return 0;
}
#endif