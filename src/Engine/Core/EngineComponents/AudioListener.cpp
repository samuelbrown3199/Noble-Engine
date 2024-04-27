#include"AudioListener.h"

#include "../Application.h"
#include "../ECS/Entity.h"
#include "../CommandTypes.h"

#include "../ECS/ComponentList.hpp"

int AudioListener::m_iCurrentListener = 0;
FMOD_VECTOR AudioListener::pos;
FMOD_VECTOR AudioListener::vel;
FMOD_VECTOR AudioListener::forward;
FMOD_VECTOR AudioListener::up;

void AudioListener::PreUpdate()
{
	m_iCurrentListener = 0;

	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	AudioManager* aManager = Application::GetApplication()->GetAudioManager();

	ComponentDatalist<AudioListener>* dataList = dynamic_cast<ComponentDatalist<AudioListener>*>(registry->GetComponentList(GetComponentID()));

	int listenerCount = 0;
	for (int i = 0; i < dataList->m_componentData.size(); i++)
	{
		if (dataList->m_componentData.at(i).m_bAvailableForReuse)
			continue;

		listenerCount++;
	}

	FMOD_System_Set3DNumListeners(aManager->GetFMODSystem(), listenerCount);
}

void AudioListener::OnUpdate()
{
	NobleRegistry* registry = Application::GetApplication()->GetRegistry();
	AudioManager* aManager = Application::GetApplication()->GetAudioManager();

	if (m_transformIndex == -1)
	{
		m_transformIndex = registry->GetComponentIndex<Transform>(m_sEntityID);
		return;
	}

	Transform* listenerTransform = registry->GetComponent<Transform>(m_transformIndex);
	if (listenerTransform == nullptr)
		return;


	pos.x = listenerTransform->m_position.x;
	pos.y = listenerTransform->m_position.y;
	pos.z = listenerTransform->m_position.z;

	vel.x = m_velocity.x;
	vel.y = m_velocity.y;
	vel.z = m_velocity.z;

	glm::vec3 lisRot = listenerTransform->m_rotation;
	lisRot.y = 0;

	glm::vec3 normFor = glm::normalize(lisRot);
	forward.x = normFor.x;
	forward.y = 0;
	forward.z = normFor.z;

	up.x = 0;
	up.y = 1;
	up.z = 0;

	FMOD_RESULT res = FMOD_System_Set3DListenerAttributes(aManager->GetFMODSystem(), m_iCurrentListener, &pos, &vel, &forward, &up);
	m_iCurrentListener++;
}

void AudioListener::DoComponentInterface()
{
	float vel[3] = { m_velocity.x,m_velocity.y ,m_velocity.z };
	if (ImGui::InputFloat3("Velocity", vel, "%.3f"))
	{
		ChangeValueCommand<glm::vec3>* command = new ChangeValueCommand<glm::vec3>(&m_velocity, glm::vec3(vel[0], vel[1], vel[2]));
		Application::GetApplication()->PushCommand(command);
	}
}