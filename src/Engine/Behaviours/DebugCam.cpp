#include "DebugCam.h"

#include "../Core/InputManager.h"
#include "../Systems/Transform.h"
#include "../Core/Renderer.h"
#include "../Core/PerformanceStats.h"

void DebugCam::Update()
{
	UpdateControls();
	UpdateCameraRotation();
}

void DebugCam::UpdateControls()
{
	Camera* ca = Renderer::GetCamera();
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	if (InputManager::GetKey(SDLK_w))
	{
		ca->m_position += (m_fMovementSpeed * (float)PerformanceStats::deltaT) * ca->m_rotation;
	}
	if (InputManager::GetKey(SDLK_s))
	{
		ca->m_position -= (m_fMovementSpeed * (float)PerformanceStats::deltaT) * ca->m_rotation;
	}
	if (InputManager::GetKey(SDLK_a))
	{
		glm::vec3 direction = glm::cross(ca->m_rotation, up);
		ca->m_position -= (m_fMovementSpeed * (float)PerformanceStats::deltaT) * direction;
	}
	if (InputManager::GetKey(SDLK_d))
	{
		glm::vec3 direction = glm::cross(ca->m_rotation, up);
		ca->m_position += (m_fMovementSpeed * (float)PerformanceStats::deltaT) * direction;
	}
	if (InputManager::GetKey(SDLK_SPACE))
	{
		ca->m_position += (m_fMovementSpeed * (float)PerformanceStats::deltaT) * up;
	}
	if (InputManager::GetKey(SDLK_x))
	{
		ca->m_position -= (m_fMovementSpeed * (float)PerformanceStats::deltaT) * up;
	}

	if (InputManager::GetKey(SDLK_p))
	{
		Renderer::SetRenderMode(GL_TRIANGLES);
	}

	if (InputManager::GetKey(SDLK_l))
	{
		Renderer::SetRenderMode(GL_LINES);
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

		yaw += xoffset;
		pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		ca->m_rotation = glm::normalize(front);
	}
	else
	{
		firstMouse = true;
	}
}