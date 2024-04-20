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
	LogInfo("Registering components from DLL");

	m_app->GetRegistry()->RegisterComponent<TestComponent>("TestComponent", false, 1024, true, true);

	LogInfo("Components registered from DLL");
}

void NobleGameDLL::RegisterBehaviours()
{

}

void NobleGameDLL::RegisterResources()
{
	LogInfo("Registering resources from DLL");

	m_app->GetRegistry()->RegisterResource("TestResource", new TestResource(), false);

	LogInfo("Resources registered from DLL");
}

void NobleGameDLL::RegisterDecriptors()
{
	LogInfo("Registering descriptors from DLL");

	LogInfo("Descriptors registered from DLL");
}

void NobleGameDLL::RegisterPushConstants()
{
	LogInfo("Registering push constants from DLL");

	LogInfo("Push constants registered from DLL");
}