#include "..\Core\Application.h"
#include "..\Core\AudioManager.h"
#include "AudioListenerSystem.h"

std::weak_ptr<SystemBase> AudioListenerSystem::self;
std::weak_ptr<SystemBase> AudioListener::componentSystem;
std::vector<AudioListener> AudioListener::componentData;

void AudioListenerSystem::PreUpdate()
{
	m_iCurrentListener = 0;
	FMOD_System_Set3DNumListeners(AudioManager::GetFMODSystem(), AudioListener::componentData.size());
}

void AudioListenerSystem::OnUpdate(AudioListener* comp)
{
	if (comp->m_listenerTransform == nullptr || Application::GetEntitiesDeleted())
	{
		comp->m_listenerTransform = Transform::GetComponent(comp->m_sEntityID);
	}

	pos.x = comp->m_listenerTransform->m_position.x;
	pos.y = comp->m_listenerTransform->m_position.y;
	pos.z = comp->m_listenerTransform->m_position.z;

	vel.x = comp->m_velocity.x;
	vel.y = comp->m_velocity.y;
	vel.z = comp->m_velocity.z;

	glm::vec3 normFor = glm::normalize(comp->m_listenerTransform->m_rotation);
	forward.x = normFor.x;
	forward.y = 0;
	forward.z = normFor.z;

	up.x = 0;
	up.y = 1;
	up.z = 0;

	FMOD_RESULT res = FMOD_System_Set3DListenerAttributes(AudioManager::GetFMODSystem(), m_iCurrentListener, &pos, &vel, &forward, &up);
	m_iCurrentListener++;
}