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
		if (!comp->cameraTransform)
		{
			Entity* camEntity = Application::GetEntity(comp->entityID);
			comp->cameraTransform = camEntity->GetComponent<NobleComponents::Transform>();
			if (!comp->cameraTransform)
			{
				comp->cameraTransform = camEntity->AddComponent<NobleComponents::Transform>();
			}
		}
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
		comp->viewMatrix = glm::mat4(1.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		comp->cameraTransform->rotation = glm::normalize(comp->cameraTransform->rotation);
		comp->viewMatrix = glm::lookAt(comp->cameraTransform->position, comp->cameraTransform->position + comp->cameraTransform->rotation, up);
	}
}