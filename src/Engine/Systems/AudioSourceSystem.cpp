#include "AudioSourceSystem.h"

std::weak_ptr<SystemBase> AudioSourceSystem::self;
std::weak_ptr<SystemBase> AudioSource::componentSystem;
std::vector<AudioSource> AudioSource::componentData;

#include "../Core/Application.h"
#include "../Core/AudioManager.h"
#include "Transform.h"
#include "../ECS/Entity.hpp"

#include <FMOD/fmod.h>
#include <FMOD/fmod_common.h>

void AudioSourceSystem::OnUpdate(AudioSource* comp)
{
	if (!comp->m_bPaused)
	{
		if (comp->channel == nullptr)
		{
			FMOD_System_PlaySound(AudioManager::GetFMODSystem(), comp->m_clip->m_sound, nullptr, true, &comp->channel);
			FMOD_Channel_SetLoopCount(comp->channel, comp->m_iLoopCount);
		}

		FMOD_Channel_SetVolume(comp->channel, comp->m_fGain);
		FMOD_Channel_SetPitch(comp->channel, comp->m_fPitch);
		//Further functions like above for 3d pos and stuff.


		FMOD_Channel_SetPaused(comp->channel, comp->m_bPaused);
	}
	else
	{
		if (comp->channel != nullptr)
		{
			//clean up stuff here to allow for other channels to be created.
		}
	}
}