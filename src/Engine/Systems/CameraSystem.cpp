#include "CameraSystem.h"
#include "..\Core\Graphics\Renderer.h"

std::weak_ptr<SystemBase> CameraSystem::self;
std::weak_ptr<SystemBase> Camera::componentSystem;
std::vector<Camera> Camera::componentData;

void CameraSystem::PreUpdate()
{
	int bestCam = -1;
	CameraState bestState = inactive;

	for(int i = 0; i < Camera::componentData.size(); i++)
	{
		if (bestState <= Camera::componentData.at(i).m_state)
		{
			bestState = Camera::componentData.at(i).m_state;
			bestCam = i;
		}

		if (bestState == editorCam)
			break;
	}

	if(bestCam != -1)
		Renderer::SetCamera(&Camera::componentData.at(bestCam));
}

void CameraSystem::OnUpdate(Camera* comp)
{
	if (comp->m_camTransform == nullptr)
	{
		comp->m_camTransform = Transform::GetComponent(comp->m_sEntityID);
		
		//Set an initial value if needed.
		if (comp->m_camTransform->m_rotation == glm::vec3(0, 0, 0))
			comp->m_camTransform->m_rotation = glm::vec3(0, 0, -1);
	}
	comp->m_camTransform->m_rotation = glm::normalize(comp->m_camTransform->m_rotation);
}