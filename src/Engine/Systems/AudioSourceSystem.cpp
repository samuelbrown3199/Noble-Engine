#include "AudioSourceSystem.h"

std::weak_ptr<SystemBase> AudioSourceSystem::self;
std::weak_ptr<SystemBase> AudioSource::componentSystem;
std::vector<AudioSource> AudioSource::componentData;

#include "../Core/Application.h"
#include "Transform.h"
#include "../ECS/Entity.hpp"

void AudioSourceSystem::OnUpdate(AudioSource* comp)
{
	if (!comp->m_sourceTransform)
		comp->m_sourceTransform = Application::GetEntity(comp->entityID)->GetComponent<Transform>();

	ALenum state;
	if (comp->m_bPlay)
	{
		if (!comp->m_bHasSourceID)
		{
			alGenSources(1, &comp->m_iSourceID);
			comp->m_bHasSourceID = true;
		}
		alGetSourcei(comp->m_iSourceID, AL_SOURCE_STATE, &state);

		if (comp->m_sourceTransform)
		{
			alSource3f(comp->m_iSourceID, AL_POSITION, comp->m_sourceTransform->m_position.x, comp->m_sourceTransform->m_position.y, comp->m_sourceTransform->m_position.z);
		}
		alSource3f(comp->m_iSourceID, AL_VELOCITY, comp->m_vVelocity.x, comp->m_vVelocity.y, comp->m_vVelocity.z);

		alSourcei(comp->m_iSourceID, AL_LOOPING, comp->m_bLoop);
		alSourcef(comp->m_iSourceID, AL_PITCH, comp->m_fPitch);
		float volume = (comp->m_fGain * AudioManager::GetAudioMixerOption(comp->m_sMixerOption)) * AudioManager::GetAudioMixerOption("master");
		alSourcef(comp->m_iSourceID, AL_GAIN, volume);
		if (state != AL_PLAYING)
		{
			alSourcei(comp->m_iSourceID, AL_BUFFER, comp->m_clip->m_iAudioClipID);
			alSourcePlay(comp->m_iSourceID);
		}
		else
		{
			if (!comp->m_bLoop)
			{
				comp->m_bPlay = false;
			}
		}
	}
	else
	{
		if (state != AL_PLAYING)
		{
			alSourceStop(comp->m_iSourceID);
			alDeleteSources(1, &comp->m_iSourceID);
			comp->m_bHasSourceID = false;
		}
	}

	AudioManager::GetOpenALError();
}