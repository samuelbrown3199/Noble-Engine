#include "Camera.h"

#include "../Application.h"
#include "../ECS/Entity.hpp"
#include "Transform.h"

#include "../ECS/ComponentList.hpp"

void Camera::PreUpdate()
{
	int bestCam = -1;
	CameraState bestState = inactive;

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	ComponentDatalist<Camera>* dataList = dynamic_cast<ComponentDatalist<Camera>*>(registry->GetComponentList(GetComponentID()));

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
		Application::GetApplication()->GetRenderer()->SetCamera(&dataList->m_componentData.at(bestCam));
}

void Camera::OnUpdate()
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	if (m_camTransformIndex == -1)
	{
		m_camTransformIndex = registry->GetComponentIndex<Transform>(m_sEntityID);
		return;
	}

	Transform* camTransform = registry->GetComponent<Transform>(m_camTransformIndex);

	if (camTransform == nullptr)
		return;

	camTransform->m_rotation = glm::normalize(camTransform->m_rotation);
}