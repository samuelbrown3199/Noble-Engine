#include <Engine/Core/Application.h>
#include <Engine/Useful.h>

#include <Engine/Systems/Sprite.h>
#include <Engine/Systems/AudioSource.h>
#include <Engine/Systems/Transform.h>


#include "Interface/MainMenu.h"
#include "World/World.hpp"
#include "World/TileChunkSystem.h"

#ifndef NDEBUG
int main()
{
    srand(time(NULL));
    std::shared_ptr<Application> app = Application::StartApplication("Tech Demo 1");
    app->BindBehaviour<World>();

    //app->BindUISystem<MainMenu>("GameData\\UI\\MainMenu.UIL")->m_bActive = true;

    Entity* floor = Application::CreateEntity();
    floor->AddComponent<Transform>(glm::vec3(0, 0, -5), glm::vec3(0, 0, 0), glm::vec3(50, 50, 5));
    floor->AddComponent<Sprite>("GameData\\Textures\\ratio.png");

    app->MainLoop();
    return 0;
}
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    std::shared_ptr<Application> app = Application::StartApplication("Tech Demo 1");
    app->BindBehaviour<World>();

    app->BindSystem<TileChunkSystem>(useBoth, "TileChunk");

    app->MainLoop();
    return 0;
}
#endif