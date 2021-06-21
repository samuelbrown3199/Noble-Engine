#include "CameraSystem.h"
#include "../Core/Application.h"

namespace NobleCore
{
	SetupComponent(NobleComponents::Camera, CameraSystem);

	void CameraSystem::OnUpdate(NobleComponents::Camera* comp)
	{
		if (comp->mainCamera)
		{
			Application::SetMainCamera(comp);
		}
		Entity* camEntity = Application::GetEntity(comp->entityID);
		comp->cameraTransform = camEntity->GetComponent<NobleComponents::Transform>();

		GenerateProjectionMatrix(comp);
		GenerateViewMatrix(comp);
	}

	void CameraSystem::GenerateProjectionMatrix(NobleComponents::Camera* comp)
	{
		comp->projectionMatrix = glm::mat4(1.0f);
		switch (comp->camMode)
		{
		case NobleComponents::CameraMode::projection:
			comp->projectionMatrix = glm::perspective(glm::radians(comp->fieldOfView), (float)Screen::GetScreenWidth() / (float)Screen::GetScreenHeight(), comp->nearPlane, comp->farPlane);
			break;
		case NobleComponents::CameraMode::orthogonal:
			comp->projectionMatrix = glm::ortho(0.0f, (float)Screen::GetScreenWidth(), (float)Screen::GetScreenHeight(), 0.0f, 0.0f, comp->farPlane);
			break;
		}
	}

	void CameraSystem::GenerateViewMatrix(NobleComponents::Camera* comp)
	{
		comp->viewMatrix = glm::mat4(1.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		comp->viewMatrix = glm::lookAt(comp->cameraTransform->position, comp->cameraTransform->position + comp->cameraTransform->rotation, up);
	}
}