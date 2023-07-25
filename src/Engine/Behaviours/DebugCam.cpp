#include "DebugCam.h"

#include "../Core/Application.h"
#include "../Core/InputManager.h"
#include "../Systems/Transform.h"
#include "../Core/Graphics/Renderer.h"
#include "../Core/PerformanceStats.h"
#include "../Systems/Camera.h"
#include "../Systems/AudioListener.h"

void DebugCam::Start()
{

	glm::vec3 pos = glm::vec3(5, 0, 0);
	glm::vec3 rot = glm::vec3(-1, 0, 0);

	Camera* curCam = Renderer::GetCamera();
	if (curCam)
	{
		pos = curCam->m_camTransform->m_position;
		rot = curCam->m_camTransform->m_rotation;
	}

	Entity* hackCam = Application::GetEntity(m_sEntityID);
	Transform* tr = hackCam->AddComponent<Transform>(pos, rot);
	hackCam->AddComponent<Camera>()->m_state = editorCam;
	hackCam->AddComponent<AudioListener>();

	tr->m_rotation.x = -1;
	tr->m_rotation.y = 0;
	tr->m_rotation.z = 0;

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
	glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

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
		if(ca->m_viewMode == orthographic)
			ca->m_viewMode = projection;
		else
			ca->m_viewMode = orthographic;
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

		yaw -= xoffset;
		pitch += yoffset;

		ca->m_camTransform->m_rotation.x += pitch;
		ca->m_camTransform->m_rotation.y += yaw;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = -cos(glm::radians(ca->m_camTransform->m_rotation.x)) * sin(glm::radians(ca->m_camTransform->m_rotation.y));
		front.y = cos(glm::radians(ca->m_camTransform->m_rotation.x)) * cos(glm::radians(ca->m_camTransform->m_rotation.y));
		front.z = sin(glm::radians(ca->m_camTransform->m_rotation.x));
		ca->m_camTransform->m_rotation = glm::normalize(front);
	}
	else
	{
		firstMouse = true;
	}
}