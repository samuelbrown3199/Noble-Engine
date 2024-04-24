#include "AudioSource.h"

#include "../Application.h"
#include "../ECS/Entity.h"

void AudioSource::OnUpdate()
{
	if (!m_bPaused)
	{
		if (m_clip == nullptr || !m_clip->m_bIsLoaded)
			return;

		NobleRegistry* registry = Application::GetApplication()->GetRegistry();
		AudioManager* aManager = Application::GetApplication()->GetAudioManager();

		if (m_transformIndex == -1)
		{
			m_transformIndex = registry->GetComponentIndex<Transform>(m_sEntityID);
			return;
		}

		Transform* transform = registry->GetComponent<Transform>(m_transformIndex);
		if (transform == nullptr)
			return;

		if (Application::GetApplication()->GetPlayMode() == false)
		{
			FMOD_Channel_Stop(channel);
			channel = nullptr;

			return;
		}

		if (channel == nullptr)
		{
			FMOD_System_PlaySound(aManager->GetFMODSystem(), m_clip->m_sound, nullptr, true, &channel);

			int loopCount = m_iLoopCount > 0 ? m_iLoopCount - 1 : m_iLoopCount;
			FMOD_Channel_SetLoopCount(channel, loopCount);
		}

		float volumeMixerValue = aManager->GetAudioMixerOption(m_sMixerOption);
		if (m_sMixerOption != "Master")
			volumeMixerValue *= aManager->GetAudioMixerOption("Master");

		FMOD_Channel_SetVolume(channel, m_fVolume * volumeMixerValue);
		FMOD_Channel_SetPitch(channel, m_fPitch);
		if (m_b3DSound)
		{
			FMOD_VECTOR pos;
			pos.x = transform->m_position.x;
			pos.y = transform->m_position.y;
			pos.z = transform->m_position.z;

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