#include "Camera.h"

#include "../Application.h"
#include "../CommandTypes.h"
#include "../ECS/Entity.h"
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
	else
		Application::GetApplication()->GetRenderer()->SetCamera(nullptr);
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

void Camera::DoComponentInterface()
{
	const char* states[] = { "Inactive", "Main Cam" };
	int currentState = m_state;
	if (ImGui::Combo("State", &currentState, states, IM_ARRAYSIZE(states)))
	{
		ChangeValueCommand<CameraState>* command = new ChangeValueCommand<CameraState>(&m_state, (CameraState)currentState);
		Application::GetApplication()->PushCommand(command);
	}

	const char* viewmodes[] = { "Projection", "Orthographic" };
	int currentViewMode = m_viewMode;
	if(ImGui::Combo("View Mode", &currentViewMode, viewmodes, IM_ARRAYSIZE(viewmodes)))
	{
		ChangeValueCommand<ViewMode>* command = new ChangeValueCommand<ViewMode>(&m_viewMode, (ViewMode)currentViewMode);
		Application::GetApplication()->PushCommand(command);
	}

	if (m_viewMode == projection)
	{
		static NobleDragFloat fovDrag;
		fovDrag.DoDragFloat("FoV", m_bInitializeInterface, &m_fov, this, 0.5f, 20.0f, 150.0f);
	}
	else
	{
		static NobleDragFloat scaleDrag;
		scaleDrag.DoDragFloat("Scale", m_bInitializeInterface, &m_scale, this, 1.0f, 3.0f, 1000.0f);
	}

	const char* drawModes[] = { "Nearest", "Linear" };
	int currentDrawMode = m_iDrawMode;
	if (ImGui::Combo("Draw Mode", &currentDrawMode, drawModes, IM_ARRAYSIZE(drawModes)))
	{
		ChangeValueCommand<int>* command = new ChangeValueCommand<int>(&m_iDrawMode, currentDrawMode);
		Application::GetApplication()->PushCommand(command);
	}

	m_bInitializeInterface = false;
}

glm::vec3 Camera::GetPosition()
{
	m_camTransformIndex = Application::GetApplication()->GetRegistry()->GetComponentIndex<Transform>(m_sEntityID);

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Transform* camTransform = registry->GetComponent<Transform>(m_camTransformIndex);
	if (camTransform == nullptr)
		return glm::vec3(0.0f, 0.0f, 0.0f);
	return camTransform->m_position;
}

glm::vec3 Camera::GetRotation()
{
	m_camTransformIndex = Application::GetApplication()->GetRegistry()->GetComponentIndex<Transform>(m_sEntityID);

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	Transform* camTransform = registry->GetComponent<Transform>(m_camTransformIndex);
	if (camTransform == nullptr)
		return glm::vec3(0.0f, 0.0f, 0.0f);
	return camTransform->m_rotation;
}