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
		void RegisterBehaviours();
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

struct TestResource : Resource
{
	std::string m_sTestString = "Hello from the DLL!";

	void DoResourceInterface()
	{
		ImGui::InputText("Test String", &m_sTestString);
	}

	TestResource()
	{
		m_resourceType = "TestResource";
	}

	~TestResource()
	{
		if (m_bIsLoaded)
		{
			OnUnload();
		}
	}

	void OnLoad() override
	{
		LogInfo("Loaded Test Resource");
		m_bIsLoaded = true;
	}

	void OnUnload() override
	{
		LogInfo("Unloaded Test Resource");
		m_bIsLoaded = false;
	}

	void AddResource(std::string path) override
	{
		ResourceManager* resourceManager = Application::GetApplication()->GetResourceManager();
		resourceManager->AddNewResource<TestResource>(path);
	}

	std::vector<std::shared_ptr<Resource>> GetResourcesOfType() override
	{
		ResourceManager* resourceManager = Application::GetApplication()->GetResourceManager();
		return resourceManager->GetAllResourcesOfType<Texture>();
	}

	std::shared_ptr<Resource> LoadFromJson(const std::string& path, const nlohmann::json& data) override
	{
		std::shared_ptr<TestResource> res = std::make_shared<TestResource>();
		
		res->m_sTestString = data["TestString"];

		return res;
	}
};