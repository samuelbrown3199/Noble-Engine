#include "DebugCam.h"

#include "../Application.h"
#include "../InputManager.h"
#include "../EngineComponents/Transform.h"
#include "../Graphics/Renderer.h"
#include "../PerformanceStats.h"
#include "../EngineComponents/Camera.h"
#include "../EngineComponents/AudioListener.h"
#include "../EngineComponents/MeshRenderer.h" //temp for ray debugging.

#include "../../Maths/Raycaster.h"

#include "../ECS/Entity.hpp"

Behaviour* DebugCam::GetAsBehaviour(std::string entityID)
{
	Entity* ent = Application::GetApplication()->GetEntity(Application::GetApplication()->GetEntityIndex(entityID));
	return ent->GetBehaviour<DebugCam>();
}

Behaviour* DebugCam::AddBehaviourToEntity(std::string entityID)
{
	Entity* ent = Application::GetApplication()->GetEntity(Application::GetApplication()->GetEntityIndex(entityID));
	return ent->AddBehaviour<DebugCam>();
}

void DebugCam::RemoveBehaviourFromEntity(std::string entityID)
{
	Entity* ent = Application::GetApplication()->GetEntity(Application::GetApplication()->GetEntityIndex(entityID));
	ent->RemoveBehaviour<DebugCam>();
}

void DebugCam::Start()
{
	glm::vec3 pos = glm::vec3(5, 0, 0);
	glm::vec3 rot = glm::vec3(0, 0, -1);

	Camera* curCam = Renderer::GetCamera();
	if (curCam == nullptr)
		return;

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Transform* camTransform = registry->GetComponent<Transform>(curCam->m_camTransformIndex);
	if (curCam && camTransform != nullptr)
	{
		pos = camTransform->m_position;
		rot = camTransform->m_rotation;
	}

	Entity* hackCam = Application::GetApplication()->GetEntity(Application::GetApplication()->GetEntityIndex(m_sEntityID));
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
	if (ca == nullptr)
		return;

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Transform* camTransform = registry->GetComponent<Transform>(ca->m_camTransformIndex);
	if (camTransform == nullptr)
		return;

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	if (InputManager::GetKeybind("Forward"))
	{
		camTransform->m_position += (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * camTransform->m_rotation;
	}
	if (InputManager::GetKeybind("Back"))
	{
		camTransform->m_position -= (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * camTransform->m_rotation;
	}
	if (InputManager::GetKeybind("Left"))
	{
		glm::vec3 direction = glm::cross(camTransform->m_rotation, up);
		camTransform->m_position -= (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * direction;
	}
	if (InputManager::GetKeybind("Right"))
	{
		glm::vec3 direction = glm::cross(camTransform->m_rotation, up);
		camTransform->m_position += (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * direction;
	}
	if (InputManager::GetKey(SDLK_SPACE))
	{
		camTransform->m_position += (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * up;
	}
	if (InputManager::GetKey(SDLK_x))
	{
		camTransform->m_position -= (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * up;
	}
	if (InputManager::GetKeyDown(SDLK_p))
	{
		if (ca->m_viewMode == orthographic)
			ca->m_viewMode = projection;
		else
			ca->m_viewMode = orthographic;
	}

	if (InputManager::GetKeybindDown("LeftMouse") && InputManager::GetKey(SDLK_r))
	{
		Ray mouseRay = Raycaster::GetRayToMousePosition();
		if (mouseRay.m_hitObject)
		{
			ResourceManager* rManager = Application::GetApplication()->GetResourceManager();

			Logger::LogInformation(FormatString("Hit entity %s! Position %.2f %.2f %.2f", mouseRay.m_hitEntity->m_sEntityID.c_str(), mouseRay.m_hitPosition.x, mouseRay.m_hitPosition.y, mouseRay.m_hitPosition.z));
			Entity* ent = Application::GetApplication()->CreateEntity();
			ent->AddComponent<Transform>(mouseRay.m_hitPosition);
			MeshRenderer* mr = ent->AddComponent<MeshRenderer>();
			mr->m_model = rManager->LoadResource<Model>("\\GameData\\Models\\cube.obj");
			mr->m_texture = rManager->LoadResource<Texture>("\\GameData\\Textures\\cottage_diffuse.png");
			mr->m_pipeline = rManager->LoadResource<Pipeline>("Standard");
		}
	}
}

void DebugCam::UpdateCameraRotation()
{
	if (InputManager::GetKeybind("RightMouse"))
	{
		Camera* ca = Renderer::GetCamera();
		if (ca == nullptr)
			return;

		newMousePos = glm::vec2(InputManager::m_iMouseX, InputManager::m_iMouseY);

		NobleRegistry* registry = Application::GetApplication()->GetRegistry();
		Transform* camTransform = registry->GetComponent<Transform>(ca->m_camTransformIndex);
		if (camTransform == nullptr)
			return;

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

		camTransform->m_rotation.x = pitch;
		camTransform->m_rotation.y = yaw;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front = glm::vec3();
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		camTransform->m_rotation = glm::normalize(front);
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