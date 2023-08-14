#include "AudioSource.h"

#include "../Core/Application.h"
#include "../ECS/Entity.hpp"

ComponentDatalist<AudioSource> AudioSource::m_componentList;

void AudioSource::AddComponent()
{
	m_componentList.AddComponent(this);
}

void AudioSource::AddComponentToEntity(std::string entityID)
{
	m_componentList.AddComponentToEntity(entityID);
	Application::GetEntity(entityID)->GetAllComponents();
}

void AudioSource::RemoveComponent(std::string entityID)
{
	m_componentList.RemoveComponent(entityID);
}

void AudioSource::RemoveAllComponents()
{
	m_componentList.RemoveAllComponents();
}

AudioSource* AudioSource::GetComponent(std::string entityID)
{
	return m_componentList.GetComponent(entityID);
}

void AudioSource::Update(bool useThreads, int maxComponentsPerThread)
{
	m_componentList.Update(useThreads, maxComponentsPerThread);
}

void AudioSource::OnUpdate()
{
	if (!m_bPaused)
	{
		if (m_clip == nullptr || !m_clip->m_bIsLoaded)
			return;

		m_sourceTransform = Application::GetEntity(m_sEntityID)->GetComponent<Transform>();
		if (m_sourceTransform == nullptr)
			return;

		if (channel == nullptr)
		{
			FMOD_System_PlaySound(AudioManager::GetFMODSystem(), m_clip->m_sound, nullptr, true, &channel);

			int loopCount = m_iLoopCount > 0 ? m_iLoopCount - 1 : m_iLoopCount;
			FMOD_Channel_SetLoopCount(channel, loopCount);
		}

		FMOD_Channel_SetVolume(channel, m_fVolume);
		FMOD_Channel_SetPitch(channel, m_fPitch);
		if (m_b3DSound)
		{
			FMOD_VECTOR pos;
			pos.x = m_sourceTransform->m_position.x;
			pos.y = m_sourceTransform->m_position.y;
			pos.z = m_sourceTransform->m_position.z;

			FMOD_VECTOR vel;
			vel.x = m_velocity.x;
			vel.y = m_velocity.y;
			vel.z = m_velocity.z;

			FMOD_Channel_Set3DAttributes(channel, &pos, &vel);
		}


		FMOD_Channel_SetPaused(channel, m_bPaused);
	}
	else
	{
		if (channel != nullptr)
		{
			//clean up stuff here to allow for other channels to be created.
			FMOD_Channel_Stop(channel);
			channel = nullptr;
		}
	}
}

void AudioSource::Render(bool useThreads, int maxComponentsPerThread) {}

void AudioSource::LoadComponentDataFromJson(nlohmann::json& j)
{
	m_componentList.LoadComponentDataFromJson(j);
}

nlohmann::json AudioSource::WriteComponentDataToJson()
{
	return m_componentList.WriteComponentDataToJson();
}