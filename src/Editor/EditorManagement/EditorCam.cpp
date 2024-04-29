#include "EditorCam.h"

#include <Engine/Core/Application.h>
#include <Engine/Core/Graphics/Renderer.h>
#include <Engine/Core/InputManager.h>

void EditorCam::Update()
{
	UpdateControls();
	UpdateCameraRotation();

	Renderer* renderer = Application::GetApplication()->GetRenderer();
	renderer->SetCamera(this); //forcing it for now, probably should only happen in edit mode/if the scene view is focused
}

void EditorCam::UpdateControls()
{
	Renderer* renderer = Application::GetApplication()->GetRenderer();
	InputManager* inputManager = Application::GetApplication()->GetInputManager();

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	if (!m_sceneView->IsWindowFocused())
		return;

	if (inputManager->GetKeybind("Forward"))
	{
		m_position += (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * m_rotation;
	}
	if (inputManager->GetKeybind("Back"))
	{
		m_position -= (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * m_rotation;
	}
	if (inputManager->GetKeybind("Left"))
	{
		glm::vec3 direction = glm::cross(m_rotation, up);
		m_position -= (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * direction;
	}
	if (inputManager->GetKeybind("Right"))
	{
		glm::vec3 direction = glm::cross(m_rotation, up);
		m_position += (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * direction;
	}
	if (inputManager->GetKey(SDLK_SPACE))
	{
		m_position += (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * up;
	}
	if (inputManager->GetKey(SDLK_x))
	{
		m_position -= (m_fMovementSpeed * (float)PerformanceStats::m_dDeltaT) * up;
	}
	if (inputManager->GetKeyDown(SDLK_p))
	{
		if (m_viewMode == orthographic)
			m_viewMode = projection;
		else
			m_viewMode = orthographic;
	}
}

void EditorCam::UpdateCameraRotation()
{
	Renderer* renderer = Application::GetApplication()->GetRenderer();
	InputManager* inputManager = Application::GetApplication()->GetInputManager();
	if (inputManager->GetKeybind("RightMouse") && m_sceneView->IsWindowFocused())
	{
		m_newMousePos = glm::vec2(inputManager->m_iMouseX, inputManager->m_iMouseY);

		if (m_bFirstMouse)
		{
			m_oldMousePos = m_newMousePos;
			m_bFirstMouse = false;
		}

		float xoffset = m_newMousePos.x - m_oldMousePos.x;
		float yoffset = m_oldMousePos.y - m_newMousePos.y; // reversed since y-coordinates go from bottom to top
		m_oldMousePos = m_newMousePos;

		xoffset *= m_fSensitivity;
		yoffset *= m_fSensitivity;

		yaw += xoffset;
		pitch += yoffset;

		m_rotation.x = pitch;
		m_rotation.y = yaw;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front = glm::vec3();
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		m_rotation = glm::normalize(front);
	}
	else
	{
		m_bFirstMouse = true;
	}
}