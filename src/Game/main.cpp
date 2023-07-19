#include <Engine/Core/Application.h>
#include <Engine/Useful.h>
#include <Engine\Behaviours\DebugCam.h>

#include <Engine/Systems/AudioSource.h>
#include <Engine/Systems/Transform.h>
#include <Engine/Resource/Scene.h>

#include "World/World.hpp"

#ifndef NDEBUG
int main()
{
    std::shared_ptr<Application> app = Application::StartApplication("Tech Demo 1");

    app->MainLoop();
    return 0;
}
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    std::shared_ptr<Application> app = Application::StartApplication("Tech Demo 1");

    app->MainLoop();
    return 0;
}
#endif