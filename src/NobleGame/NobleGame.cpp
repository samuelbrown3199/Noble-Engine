#include "NobleGame.h"

#include <iostream>

__declspec(dllexport) void TestFunction(std::string text)
{
	Logger::m_bUseLogging = true;
	Logger::LogInformation(text);
}

__declspec(dllexport) void TestRegistry(std::shared_ptr<Application> app, NobleRegistry* registry)
{
	registry->RegisterComponent<TestComponent>("TestComponent", false, 1024, true, true);
}