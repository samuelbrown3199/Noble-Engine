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
	Entity* hackCam = Application::GetEntity(m_sEntityID);
	hackCam->m_sEntityName = "Hack Cam";
	hackCam->AddComponent<Transform>(glm::vec3(2,2,2));
	hackCam->AddComponent<Camera>()->m_bIsMainCam = true;
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

	if (InputManager::GetKey(SDLK_w))
	{
		ca->m_camTransform->m_position += (m_fMovementSpeed * (float)PerformanceStats::deltaT) * ca->m_camTransform->m_rotation;
	}
	if (InputManager::GetKey(SDLK_s))
	{
		ca->m_camTransform->m_position -= (m_fMovementSpeed * (float)PerformanceStats::deltaT) * ca->m_camTransform->m_rotation;
	}
	if (InputManager::GetKey(SDLK_a))
	{
		glm::vec3 direction = glm::cross(ca->m_camTransform->m_rotation, up);
		ca->m_camTransform->m_position -= (m_fMovementSpeed * (float)PerformanceStats::deltaT) * direction;
	}
	if (InputManager::GetKey(SDLK_d))
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
}

void DebugCam::UpdateCameraRotation()
{
	if (InputManager::GetMouseButton(1))
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
		/*front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));*/
		ca->m_camTransform->m_rotation = glm::normalize(front);
	}
	else
	{
		firstMouse = true;
	}
}