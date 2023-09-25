#include "Camera.h"

#include "../Core/Application.h"
#include "../ECS/Entity.hpp"
#include "Transform.h"

#include "../ECS/ComponentList.hpp"

void Camera::PreUpdate()
{
	int bestCam = -1;
	CameraState bestState = inactive;

	ComponentDatalist<Camera>* dataList = dynamic_cast<ComponentDatalist<Camera>*>(NobleRegistry::GetComponentList(GetComponentID()));

	for (int i = 0; i < dataList->m_componentData.size(); i++)
	{
		if (dataList->m_componentData.at(i).m_bAvailableForReuse)
			continue;

		if (bestState <= dataList->m_componentData.at(i).m_state)
		{
			bestState = dataList->m_componentData.at(i).m_state;
			bestCam = i;
		}

		if (bestState == editorCam)
			break;
	}

	if (bestCam != -1)
		Renderer::SetCamera(&dataList->m_componentData.at(bestCam));
}

void Camera::OnUpdate()
{
	if (m_camTransform == nullptr)
	{
		m_camTransform = Application::GetEntity(m_sEntityID)->GetComponent<Transform>();

		if (m_camTransform == nullptr)
			return;

		//Set an initial value if needed.
		if (m_camTransform->m_rotation == glm::vec3(0, 0, 0))
			m_camTransform->m_rotation = glm::vec3(0, 0, -1);
	}
	m_camTransform->m_rotation = glm::normalize(m_camTransform->m_rotation);
}