#include "NobleGame.h"

#include <iostream>

__declspec(dllexport) void dllMain(std::shared_ptr<Application> application, bool useLogging)
{
	NobleGameDLL gameDLL;
	gameDLL.InitializeDLL(application, useLogging);
}

__declspec(dllexport) void TestFunction(std::string text)
{
	Logger::m_bUseLogging = true;
	Logger::LogInformation(text);
}

__declspec(dllexport) void TestRegistry(std::shared_ptr<Application> app, NobleRegistry* registry)
{
	registry->RegisterComponent<TestComponent>("TestComponent", false, 1024, true, true);
}

void NobleGameDLL::InitializeDLL(std::shared_ptr<Application> application, bool useLogging)
{
	Logger::m_bUseLogging = useLogging;
	Logger::LogInformation("Initializing DLL");

	m_app = application;
	m_app->m_self = application;
	ImGui::SetCurrentContext(m_app->GetRenderer()->GetImguiContext());

	RegisterComponents();
	RegisterBehaviours();
	RegisterResources();
	RegisterDecriptors();
	RegisterPushConstants();

	Logger::LogInformation("DLL Initialized");
}

void NobleGameDLL::RegisterComponents()
{
	Logger::LogInformation("Registering components from DLL");

	m_app->GetRegistry()->RegisterComponent<TestComponent>("TestComponent", false, 1024, true, true);

	Logger::LogInformation("Components registered from DLL");
}

void NobleGameDLL::RegisterBehaviours()
{

}

void NobleGameDLL::RegisterResources()
{
	Logger::LogInformation("Registering resources from DLL");

	m_app->GetRegistry()->RegisterResource("TestResource", new TestResource(), false);

	Logger::LogInformation("Resources registered from DLL");
}

void NobleGameDLL::RegisterDecriptors()
{
	Logger::LogInformation("Registering descriptors from DLL");

	Logger::LogInformation("Descriptors registered from DLL");
}

void NobleGameDLL::RegisterPushConstants()
{
	Logger::LogInformation("Registering push constants from DLL");

	Logger::LogInformation("Push constants registered from DLL");
}