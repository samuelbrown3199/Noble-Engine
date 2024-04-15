#include "NobleGame.h"

#include <iostream>

__declspec(dllexport) void dllMain(std::shared_ptr<Application> application)
{
	NobleGameDLL gameDLL;
	gameDLL.InitializeDLL(application);
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

void NobleGameDLL::InitializeDLL(std::shared_ptr<Application> application)
{
	m_app = application;
	m_app->m_self = application;
	ImGui::SetCurrentContext(m_app->GetRenderer()->GetImguiContext());
	//Logger::m_bUseLogging = m_app->GetLogger()->m_bUseLogging;
	//Logger::LogInformation("Initializing DLL");

	RegisterComponents();
}

void NobleGameDLL::RegisterComponents()
{
	m_app->GetRegistry()->RegisterComponent<TestComponent>("TestComponent", false, 1024, true, true);
}