#include "NobleGame.h"

#include <iostream>

__declspec(dllexport) void dllMain(std::shared_ptr<Application> application)
{
	NobleGameDLL gameDLL;
	gameDLL.InitializeDLL(application);
}

void NobleGameDLL::InitializeDLL(std::shared_ptr<Application> application)
{
	m_app = application;
	m_app->m_self = application;

	LogInfo("Initializing DLL");

	ImGui::SetCurrentContext(m_app->GetRenderer()->GetImguiContext());

	RegisterComponents();
	RegisterBehaviours();
	RegisterResources();
	RegisterDecriptors();
	RegisterPushConstants();

	LogInfo("DLL Initialized");
}

void NobleGameDLL::RegisterComponents()
{
	LogTrace("Registering components from DLL");

	m_app->GetRegistry()->RegisterComponent<TestComponent>("TestComponent", false, 1024, true, true);

	LogTrace("Components registered from DLL");
}

void NobleGameDLL::RegisterBehaviours()
{
	LogTrace("Registering behaviours from DLL");

	LogTrace("Behaviours registered from DLL");
}

void NobleGameDLL::RegisterResources()
{
	LogTrace("Registering resources from DLL");

	m_app->GetRegistry()->RegisterResource("TestResource", new TestResource(), false);

	LogTrace("Resources registered from DLL");
}

void NobleGameDLL::RegisterDecriptors()
{
	LogTrace("Registering descriptors from DLL");

	LogTrace("Descriptors registered from DLL");
}

void NobleGameDLL::RegisterPushConstants()
{
	LogTrace("Registering push constants from DLL");

	LogTrace("Push constants registered from DLL");
}