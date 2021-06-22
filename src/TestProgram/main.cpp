#include <NobleEngine/Core/Application.h>
#include <NobleEngine/Core/ResourceManager.h>
#include <NobleEngine/Components/Transform.hpp>
#include <NobleEngine/Components/Camera.hpp>
#include <NobleEngine/Components/Mesh.hpp>
#include <NobleEngine/Components/StaticTransform.hpp>
#include <NobleEngine/Components/PhysicsBody.hpp>

#include <NobleEngine/ResourceManagement/ShaderProgram.hpp>

#include "FlyingCamSystem.h"

using namespace NobleCore;
using namespace NobleComponents;
using namespace NobleResources;
int main()
{
	std::shared_ptr<Application> app = Application::InitializeEngine("Test Program", GraphicsAPI::OpenGL, 1366, 768);

	std::shared_ptr<Shader> vertexShader = ResourceManager::LoadResource<Shader>("Resources\\Shaders\\standard.vs");
	std::shared_ptr<Shader> fragmentShader = ResourceManager::LoadResource<Shader>("Resources\\Shaders\\standard.fs");

	std::shared_ptr<ShaderProgram> temp = ResourceManager::CreateShaderProgram();
	temp->BindShader(vertexShader, GL_VERTEX_SHADER);
	temp->BindShader(fragmentShader, GL_FRAGMENT_SHADER);
	temp->LinkShaderProgram(temp);

	std::shared_ptr<Texture> testTex = ResourceManager::LoadResource<Texture>("Resources\\Textures\\test.png");

	Entity* test = Application::CreateEntity();
	test->AddComponent<Transform>(glm::vec3(0, 0, 5), glm::vec3(0,0,-1));
	test->AddComponent<Camera>(true, CameraMode::projection);
	test->AddComponent<FlyingCam>();

	int amount = 5;
	for (int x = 0; x < amount; x++)
	{
		for (int y = 0; y < amount; y++)
		{
			for (int z = 0; z < amount; z++)
			{
				Entity* renderTest = Application::CreateEntity();
				renderTest->AddComponent<PhysicsBody>(box, 10, glm::vec3(0 + (x * 10), 5 + (y * 10), -10 +(z*10)), glm::vec3(40, 25, 73), glm::vec3(1, 1, 1));
				renderTest->AddComponent<Mesh>(Physics, ResourceManager::LoadResource<Model>("Resources\\Models\\cube.obj"), temp, testTex);
			}
		}
	}
	Entity* floor = Application::CreateEntity();
	floor->AddComponent<PhysicsBody>(box, 0, glm::vec3(0 , 0 , 0), glm::vec3(0, 0, 0), glm::vec3(100, 1, 100));
	floor->AddComponent<Mesh>(Physics, ResourceManager::LoadResource<Model>("Resources\\Models\\cube.obj"), temp, testTex);

	app->BindSystem<FlyingCamSystem>(SystemUsage::useUpdate);
	app->MainEngineLoop();

	return 0;
}