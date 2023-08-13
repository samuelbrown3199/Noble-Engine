#include "AudioSource.h"

#include "../Core/Application.h"
#include "../ECS/Entity.hpp"

std::vector<AudioSource> AudioSource::m_componentData;

void AudioSource::AddComponent()
{
	m_componentData.push_back(*this);
}

void AudioSource::RemoveComponent(std::string entityID)
{
	for (int i = 0; i < m_componentData.size(); i++)
	{
		if (m_componentData.at(i).m_sEntityID == entityID)
			m_componentData.erase(m_componentData.begin() + i);
	}
}

void AudioSource::RemoveAllComponents()
{
	m_componentData.clear();
}

AudioSource* AudioSource::GetComponent(std::string entityID)
{
	for (int i = 0; i < m_componentData.size(); i++)
	{
		if (m_componentData.at(i).m_sEntityID == entityID)
			return &m_componentData.at(i);
	}

	return nullptr;
}

void AudioSource::Update(bool useThreads, int maxComponentsPerThread)
{
	if (!useThreads)
	{
		for (int i = 0; i < m_componentData.size(); i++)
		{
			m_componentData.at(i).OnUpdate();
		}
	}
	else
	{
		double amountOfThreads = ceil(m_componentData.size() / maxComponentsPerThread) + 1;
		for (int i = 0; i < amountOfThreads; i++)
		{
			int buffer = maxComponentsPerThread * i;
			auto th = ThreadingManager::EnqueueTask([&] { ThreadUpdate(buffer, maxComponentsPerThread); });
		}
	}
}

void AudioSource::ThreadUpdate(int _buffer, int _amount)
{
	int maxCap = _buffer + _amount;
	for (size_t co = _buffer; co < maxCap; co++)
	{
		if (co >= m_componentData.size())
			break;

		m_componentData.at(co).OnUpdate();
	}
}

void AudioSource::OnUpdate()
{
	if (!m_bPaused)
	{
		if (m_clip == nullptr || !m_clip->m_bIsLoaded)
			false;

		if (m_sourceTransform == nullptr || Application::GetEntitiesDeleted())
		{
			m_sourceTransform = Application::GetEntity(m_sEntityID)->GetComponent<Transform>();
			return;
		}

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
void AudioSource::ThreadRender(int _buffer, int _amount) {}

void AudioSource::LoadComponentDataFromJson(nlohmann::json& j)
{
	for (auto it : j.items())
	{
		AudioSource component;
		component.m_sEntityID = it.key();
		component.FromJson(j[it.key()]);

		m_componentData.push_back(component);
	}
}

nlohmann::json AudioSource::WriteComponentDataToJson()
{
	nlohmann::json data;

	for (int i = 0; i < m_componentData.size(); i++)
	{
		data[m_componentData.at(i).m_sEntityID] = m_componentData.at(i).WriteJson();
	}

	return data;
}