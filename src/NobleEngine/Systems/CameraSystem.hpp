#include "../ECS/System.hpp"
#include "../Components/Camera.hpp"

#include<glm/glm.hpp>
#include<glm/ext.hpp>

namespace NobleCore
{
	struct CameraSystem : public System<NobleComponents::Camera>
	{
		void OnUpdate(NobleComponents::Camera* comp)
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
			comp->cameraMatrix = glm::mat4(1.0f);
			switch (comp->camMode)
			{
			case NobleComponents::CameraMode::projection:
				comp->cameraMatrix = glm::perspective(glm::radians(comp->fieldOfView), (float)Screen::GetScreenWidth() / (float)Screen::GetScreenHeight(), comp->nearPlane, comp->farPlane);
				break;
			case NobleComponents::CameraMode::orthogonal:
				comp->cameraMatrix = glm::ortho(0.0f, (float)Screen::GetScreenWidth(), (float)Screen::GetScreenHeight(), 0.0f, 0.0f, comp->farPlane);
				break;
			}
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
			comp->cameraTransform->rotation = glm::normalize(comp->cameraTransform->rotation);
			comp->cameraMatrix = glm::lookAt(comp->cameraTransform->position, comp->cameraTransform->position + comp->cameraTransform->rotation, up);
		}
	};

	SetupComponent(NobleComponents::Camera, CameraSystem);
}