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

#include "../ECS/Entity.h"

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
	Renderer* renderer = Application::GetApplication()->GetRenderer();
	InputManager* inputManager = Application::GetApplication()->GetInputManager();

	glm::vec3 pos = glm::vec3(5, 0, 0);
	glm::vec3 rot = glm::vec3(0, 0, -1);

	Camera* curCam = renderer->GetCamera();
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

	oldMousePos = glm::vec2(inputManager->m_iMouseX, inputManager->m_iMouseY);
	newMousePos = glm::vec2(inputManager->m_iMouseX, inputManager->m_iMouseY);
}

void DebugCam::Update()
{
	UpdateControls();
	UpdateCameraRotation();
}

void DebugCam::UpdateControls()
{
	Renderer* renderer = Application::GetApplication()->GetRenderer();
	InputManager* inputManager = Application::GetApplication()->GetInputManager();
	Camera* ca = renderer->GetCamera();
	if (ca == nullptr)
		return;

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Transform* camTransform = registry->GetComponent<Transform>(ca->m_camTransformIndex);
	if (camTransform == nullptr)
		return;

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	if (inputManager->GetKeybind("Forward"))
	{
		camTransform->m_position += (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * camTransform->m_rotation;
	}
	if (inputManager->GetKeybind("Back"))
	{
		camTransform->m_position -= (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * camTransform->m_rotation;
	}
	if (inputManager->GetKeybind("Left"))
	{
		glm::vec3 direction = glm::cross(camTransform->m_rotation, up);
		camTransform->m_position -= (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * direction;
	}
	if (inputManager->GetKeybind("Right"))
	{
		glm::vec3 direction = glm::cross(camTransform->m_rotation, up);
		camTransform->m_position += (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * direction;
	}
	if (inputManager->GetKey(SDLK_SPACE))
	{
		camTransform->m_position += (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * up;
	}
	if (inputManager->GetKey(SDLK_x))
	{
		camTransform->m_position -= (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * up;
	}
	if (inputManager->GetKeyDown(SDLK_p))
	{
		if (ca->m_viewMode == orthographic)
			ca->m_viewMode = projection;
		else
			ca->m_viewMode = orthographic;
	}
}

void DebugCam::UpdateCameraRotation()
{
	Renderer* renderer = Application::GetApplication()->GetRenderer();
	InputManager* inputManager = Application::GetApplication()->GetInputManager();
	if (inputManager->GetKeybind("RightMouse"))
	{
		Camera* ca = renderer->GetCamera();
		if (ca == nullptr)
			return;

		newMousePos = glm::vec2(inputManager->m_iMouseX, inputManager->m_iMouseY);

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