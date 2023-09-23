#include "DebugCam.h"

#include "../Core/Application.h"
#include "../Core/InputManager.h"
#include "../Components/Transform.h"
#include "../Core/Graphics/Renderer.h"
#include "../Core/PerformanceStats.h"
#include "../Components/Camera.h"
#include "../Components/AudioListener.h"
#include "../Components/MeshRenderer.h" //temp for ray debugging.

#include "../Maths/Raycaster.h"

#include "../ECS/Entity.hpp"

Behaviour* DebugCam::GetAsBehaviour(std::string entityID)
{
	Entity* ent = Application::GetEntity(entityID);
	return ent->GetBehaviour<DebugCam>();
}

Behaviour* DebugCam::AddBehaviourToEntity(std::string entityID)
{
	Entity* ent = Application::GetEntity(entityID);
	return ent->AddBehaviour<DebugCam>();
}

void DebugCam::RemoveBehaviourFromEntity(std::string entityID)
{
	Entity* ent = Application::GetEntity(entityID);
	ent->RemoveBehaviour<DebugCam>();
}

void DebugCam::Start()
{
	glm::vec3 pos = glm::vec3(5, 0, 0);
	glm::vec3 rot = glm::vec3(0, 0, -1);

	Camera* curCam = Renderer::GetCamera();
	if (curCam && curCam->m_camTransform != nullptr)
	{
		pos = curCam->m_camTransform->m_position;
		rot = curCam->m_camTransform->m_rotation;
	}

	Entity* hackCam = Application::GetEntity(m_sEntityID);
	Transform* tr = hackCam->AddComponent<Transform>(pos, rot);
	Camera* editCam = hackCam->AddComponent<Camera>();
	editCam->m_state = editorCam;

	hackCam->AddComponent<AudioListener>();

	oldMousePos = glm::vec2(InputManager::m_iMouseX, InputManager::m_iMouseY);
	newMousePos = glm::vec2(InputManager::m_iMouseX, InputManager::m_iMouseY);
}

void DebugCam::Update()
{
	UpdateControls();
	UpdateCameraRotation();
}

void DebugCam::UpdateControls()
{
	Camera* ca = Renderer::GetCamera();
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	if (InputManager::GetKeybind("Forward"))
	{
		ca->m_camTransform->m_position += (m_fMovementSpeed * (float)PerformanceStats::deltaT) * ca->m_camTransform->m_rotation;
	}
	if (InputManager::GetKeybind("Back"))
	{
		ca->m_camTransform->m_position -= (m_fMovementSpeed * (float)PerformanceStats::deltaT) * ca->m_camTransform->m_rotation;
	}
	if (InputManager::GetKeybind("Left"))
	{
		glm::vec3 direction = glm::cross(ca->m_camTransform->m_rotation, up);
		ca->m_camTransform->m_position -= (m_fMovementSpeed * (float)PerformanceStats::deltaT) * direction;
	}
	if (InputManager::GetKeybind("Right"))
	{
		glm::vec3 direction = glm::cross(ca->m_camTransform->m_rotation, up);
		ca->m_camTransform->m_position += (m_fMovementSpeed * (float)PerformanceStats::deltaT) * direction;
	}
	if (InputManager::GetKey(SDLK_SPACE))
	{
		ca->m_camTransform->m_position += (m_fMovementSpeed * (float)PerformanceStats::deltaT) * up;
	}
	if (InputManager::GetKey(SDLK_x))
	{
		ca->m_camTransform->m_position -= (m_fMovementSpeed * (float)PerformanceStats::deltaT) * up;
	}
	if (InputManager::GetKeyDown(SDLK_p))
	{
		if (ca->m_viewMode == orthographic)
			ca->m_viewMode = projection;
		else
			ca->m_viewMode = orthographic;
	}

	if (InputManager::GetKeybindDown("LeftMouse"))
	{
		Ray mouseRay = Raycaster::GetRayToInDirection(ca->m_camTransform->m_position, ca->m_camTransform->m_rotation);

		if (mouseRay.m_hitObject)
		{
			Logger::LogInformation(FormatString("Hit entity %s! Position %.2f %.2f %.2f", mouseRay.m_hitEntity->m_sEntityID.c_str(), mouseRay.m_hitPosition.x, mouseRay.m_hitPosition.y, mouseRay.m_hitPosition.z));

			Entity* ent = Application::CreateEntity();
			ent->AddComponent<Transform>(mouseRay.m_hitPosition);

			MeshRenderer* mr = ent->AddComponent<MeshRenderer>();
			mr->m_model = ResourceManager::LoadResource<Model>("\\GameData\\Models\\cube.obj");
			mr->m_texture = ResourceManager::LoadResource<Texture>("\\GameData\\Textures\\cottage_diffuse.png");
			mr->m_shader = ResourceManager::GetShaderProgram("Standard");
		}
	}
}

void DebugCam::UpdateCameraRotation()
{
	if (InputManager::GetKeybind("RightMouse"))
	{
		Camera* ca = Renderer::GetCamera();
		newMousePos = glm::vec2(InputManager::m_iMouseX, InputManager::m_iMouseY);

		if (firstMouse)
		{
			oldMousePos = newMousePos;
			firstMouse = false;
		}

		float xoffset = newMousePos.x - oldMousePos.x;
		float yoffset = oldMousePos.y - newMousePos.y; // reversed since y-coordinates go from bottom to top
		oldMousePos = newMousePos;

		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		ca->m_camTransform->m_rotation.x = pitch;
		ca->m_camTransform->m_rotation.y = yaw;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		ca->m_camTransform->m_rotation = glm::normalize(front);
	}
	else
	{
		firstMouse = true;
	}
}


void DebugCam::LoadBehaviourFromJson(std::string entityID, nlohmann::json& j)
{
	DebugCam* cam = dynamic_cast<DebugCam*>(AddBehaviourToEntity(entityID));

	cam->m_fMovementSpeed = j["MoveSpeed"];
	cam->sensitivity = j["Sensitivity"];
}

nlohmann::json DebugCam::WriteBehaviourToJson()
{
	nlohmann::json data;

	data["MoveSpeed"] = m_fMovementSpeed;
	data["Sensitivity"] = sensitivity;

	return data;
}