#include <NobleEngine/Core/Application.h>
#include <NobleEngine/Core/ResourceManager.h>
#include <NobleEngine/Components/Transform.hpp>
#include <NobleEngine/Components/Camera.hpp>
#include <NobleEngine/Components/Mesh.hpp>
#include <NobleEngine/Components/StaticTransform.hpp>

#include <NobleEngine/ResourceManagement/ShaderProgram.hpp>

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

	Entity* test = Application::CreateEntity();
	test->AddComponent<Transform>();
	test->AddComponent<Camera>(true, CameraMode::projection);


	Entity* renderTest = Application::CreateEntity();
	renderTest->AddComponent<Transform>(glm::vec3(0, 0 , -10), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	renderTest->AddComponent<Mesh>(ResourceManager::LoadResource<Model>("Resources\\Models\\cube.obj"), temp);

	app->MainEngineLoop();

	return 0;
}