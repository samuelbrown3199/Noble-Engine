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
}

extern "C"
{
	class NobleGameDLL
	{

	private:

		std::shared_ptr<Application> m_app;

		void RegisterComponents();
		void RegisterResources();
		void RegisterDecriptors();
		void RegisterPushConstants();

	public:

		void InitializeDLL(std::shared_ptr<Application> application);
	};
}

struct TestComponent : Component
{
	int m_iTestValue = 0;

	void OnUpdate() override
	{
		LogInfo("Hello from the DLL!");
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