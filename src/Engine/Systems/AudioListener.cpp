#include"AudioListener.h"

#include "../Core/Application.h"
#include "../ECS/Entity.hpp"

int AudioListener::m_iCurrentListener = 0;
FMOD_VECTOR AudioListener::pos;
FMOD_VECTOR AudioListener::vel;
FMOD_VECTOR AudioListener::forward;
FMOD_VECTOR AudioListener::up;
std::vector<AudioListener> AudioListener::m_componentData;

void AudioListener::AddComponent()
{
	m_componentData.push_back(*this);
}

void AudioListener::RemoveComponent(std::string entityID)
{
	for (int i = 0; i < m_componentData.size(); i++)
	{
		if (m_componentData.at(i).m_sEntityID == entityID)
			m_componentData.erase(m_componentData.begin() + i);
	}
}

void AudioListener::RemoveAllComponents()
{
	m_componentData.clear();
}

AudioListener* AudioListener::GetComponent(std::string entityID)
{
	for (int i = 0; i < m_componentData.size(); i++)
	{
		if (m_componentData.at(i).m_sEntityID == entityID)
			return &m_componentData.at(i);
	}

	return nullptr;
}

void AudioListener::PreUpdate()
{
	m_iCurrentListener = 0;
	FMOD_System_Set3DNumListeners(AudioManager::GetFMODSystem(), m_componentData.size());
}

void AudioListener::Update(bool useThreads, int maxComponentsPerThread)
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

void AudioListener::ThreadUpdate(int _buffer, int _amount)
{
	int maxCap = _buffer + _amount;
	for (size_t co = _buffer; co < maxCap; co++)
	{
		if (co >= m_componentData.size())
			break;

		m_componentData.at(co).OnUpdate();
	}
}

void AudioListener::OnUpdate()
{
	if (m_listenerTransform == nullptr || Application::GetEntitiesDeleted())
	{
		m_listenerTransform = Application::GetEntity(m_sEntityID)->GetComponent<Transform>();
	}

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
void AudioListener::ThreadRender(int _buffer, int _amount) {}

void AudioListener::LoadComponentDataFromJson(nlohmann::json& j)
{
	for (auto it : j.items())
	{
		AudioListener component;
		component.m_sEntityID = it.key();
		component.FromJson(j[it.key()]);

		m_componentData.push_back(component);
	}
}

nlohmann::json AudioListener::WriteComponentDataToJson()
{
	nlohmann::json data;

	for (int i = 0; i < m_componentData.size(); i++)
	{
		data[m_componentData.at(i).m_sEntityID] = m_componentData.at(i).WriteJson();
	}

	return data;
}