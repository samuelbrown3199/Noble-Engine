#pragma once

#include <Engine/Core/EngineComponents/Camera.h>
#include <glm/glm.hpp>

#include "../Interface/SceneViewWindow.h"

struct EditorCam : public CameraBase
{
	EditorCam(SceneViewWindow* sceneView)
	{
		m_sceneView = sceneView;
		m_state = editorCam;
	}

	float m_fMovementSpeed = 16.0f, m_fSensitivity = 0.1f;
	glm::vec2 m_newMousePos = glm::vec2(), m_oldMousePos = glm::vec2();
	bool m_bFirstMouse = false;
	float pitch = 0.0f, yaw = -90.0f;

	SceneViewWindow* m_sceneView = nullptr;

	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	void Update();
	void UpdateControls();
	void UpdateCameraRotation();

	virtual glm::vec3 GetPosition() override
	{
		return m_position;
	}

	virtual glm::vec3 GetRotation() override
	{
		return m_rotation;
	}

	virtual void SetPosition(glm::vec3 position) override
	{
		m_position = position;
	}

	virtual void SetRotation(glm::vec3 rotation) override
	{
		m_rotation = rotation;
	}
};