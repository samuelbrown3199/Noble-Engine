#pragma once

#include <string>

#include <Engine/Core/Logger.h>
#include <Engine/Core/Registry.h>
#include <Engine/Core/ECS/Component.h>

extern "C"
{
	__declspec(dllexport) void TestFunction(std::string text);

	__declspec(dllexport) void TestRegistry(std::shared_ptr<Application> app, NobleRegistry* registry);
}

struct TestComponent : Component
{
	void OnUpdate() override
	{
		Logger::LogInformation("Hello from the DLL!");
	}

	std::string GetComponentID() override
	{
		return "TestComponent";
	}
};