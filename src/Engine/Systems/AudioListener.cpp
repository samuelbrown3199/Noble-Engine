#include"AudioListener.h"

#include "../Core/Application.h"
#include "../ECS/Entity.hpp"

int AudioListener::m_iCurrentListener = 0;
FMOD_VECTOR AudioListener::pos;
FMOD_VECTOR AudioListener::vel;
FMOD_VECTOR AudioListener::forward;
FMOD_VECTOR AudioListener::up;

ComponentDatalist<AudioListener> AudioListener::m_componentList;

void AudioListener::AddComponent()
{
	m_componentList.AddComponent(this);
}

void AudioListener::RemoveComponent(std::string entityID)
{
	m_componentList.RemoveComponent(entityID);
}

void AudioListener::RemoveAllComponents()
{
	m_componentList.RemoveAllComponents();
}

AudioListener* AudioListener::GetComponent(std::string entityID)
{
	return m_componentList.GetComponent(entityID);
}

void AudioListener::PreUpdate()
{
	m_iCurrentListener = 0;
	FMOD_System_Set3DNumListeners(AudioManager::GetFMODSystem(), m_componentList.m_componentData.size());
}

void AudioListener::Update(bool useThreads, int maxComponentsPerThread)
{
	m_componentList.Update(useThreads, maxComponentsPerThread);
}

void AudioListener::OnUpdate()
{
	m_listenerTransform = Application::GetEntity(m_sEntityID)->GetComponent<Transform>();
	if (m_listenerTransform == nullptr)
		return;


	pos.x = m_listenerTransform->m_position.x;
	pos.y = m_listenerTransform->m_position.y;
	pos.z = m_listenerTransform->m_position.z;

	vel.x = m_velocity.x;
	vel.y = m_velocity.y;
	vel.z = m_velocity.z;

	glm::vec3 normFor = glm::normalize(m_listenerTransform->m_rotation);
	forward.x = normFor.x;
	forward.y = 0;
	forward.z = normFor.z;

	up.x = 0;
	up.y = 1;
	up.z = 0;

	FMOD_RESULT res = FMOD_System_Set3DListenerAttributes(AudioManager::GetFMODSystem(), m_iCurrentListener, &pos, &vel, &forward, &up);
	m_iCurrentListener++;
}

void AudioListener::Render(bool useThreads, int maxComponentsPerThread) {}

void AudioListener::LoadComponentDataFromJson(nlohmann::json& j)
{
	m_componentList.LoadComponentDataFromJson(j);
}

nlohmann::json AudioListener::WriteComponentDataToJson()
{
	return m_componentList.WriteComponentDataToJson();
}