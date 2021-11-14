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

	Entity* test = Application::CreateEntity();
	test->AddComponent<Transform>(glm::vec3(0, 0, 5), glm::vec3(0,0,-1));
	//test->AddComponent<Camera>(true, CameraMode::projection);

	app->BindSystem<FlyingCamSystem>(SystemUsage::useUpdate);
	app->MainEngineLoop();

	return 0;
}