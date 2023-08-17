#include <Engine/Core/Application.h>
#include "Interface/EditorUI.h"

#include <Windows.h>

void StartupEditor()
{
#ifndef NDEBUG
    std::string windowName = "Noble Editor Debug";
#else
    std::string windowName = "Noble Editor";
#endif

    std::shared_ptr<Application> app = Application::StartApplication(windowName);

    app->SetPlayMode(false);

    std::shared_ptr<EditorUI> ui = Application::BindDebugUI<EditorUI>();
    ui->m_uiOpen = true;

    app->MainLoop();
}

#ifndef NDEBUG
int main()
{
    StartupEditor();
    return 0;
}
#else
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    StartupEditor();
    return 0;
}
#endif