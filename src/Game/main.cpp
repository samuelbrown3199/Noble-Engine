#include <Engine/Core/Application.h>
#include <Engine/Core/SceneManager.h>

#include <Windows.h>

#ifndef NDEBUG
int main()
{
    std::shared_ptr<Application> app = Application::StartApplication("Tech Demo 1");

    SceneManager::LoadScene("\\GameData\\Lmao.nsc");

    app->MainLoop();
    return 0;
}
#else
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    std::shared_ptr<Application> app = Application::StartApplication("Tech Demo 1");

    SceneManager::LoadScene("\\GameData\\Lmao.nsc");

    app->MainLoop();
    return 0;
}
#endif