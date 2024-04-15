#pragma once

#include <memory>
#include <string>

#include <Engine/Core/NobleDLL.h>
#include <Engine/Core/Logger.h>
#include <Engine/Core/Registry.h>
#include <Engine/Core/ECS/Component.h>

extern "C"
{
	__declspec(dllexport) void dllMain(std::shared_ptr<Application> application);

	__declspec(dllexport) void TestFunction(std::string text);

	__declspec(dllexport) void TestRegistry(std::shared_ptr<Application> app, NobleRegistry* registry);
}

extern "C"
{
	class NobleGameDLL
	{

	private:

		std::shared_ptr<Application> m_app;

		void RegisterComponents();

	public:

		void InitializeDLL(std::shared_ptr<Application> application);
	};
}

struct TestComponent : Component
{
	int m_iTestValue = 0;

	void OnUpdate() override
	{
		Logger::LogInformation("Hello from the DLL!");
	}

	void DoComponentInterface()
	{
		ImGui::DragInt("Test Value", &m_iTestValue);
	}

	std::string GetComponentID() override
	{
		return "TestComponent";
	}
};