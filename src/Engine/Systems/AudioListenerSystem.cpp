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

	FMOD_VECTOR pos;
	pos.x = comp->m_listenerTransform->m_position.x;
	pos.y = comp->m_listenerTransform->m_position.y;
	pos.z = comp->m_listenerTransform->m_position.z;

	FMOD_VECTOR vel;
	vel.x = comp->m_velocity.x;
	vel.y = comp->m_velocity.y;
	vel.z = comp->m_velocity.z;

	FMOD_VECTOR forward;
	forward.x = comp->m_listenerTransform->m_rotation.x;
	forward.y = comp->m_listenerTransform->m_rotation.y;
	forward.z = comp->m_listenerTransform->m_rotation.z;

	FMOD_VECTOR up;
	up.x = comp->m_up.x;
	up.y = comp->m_up.y;
	up.z = comp->m_up.z;

	FMOD_System_Set3DListenerAttributes(AudioManager::GetFMODSystem(), m_iCurrentListener, &pos, &vel, &forward, &up);

	m_iCurrentListener++;
}