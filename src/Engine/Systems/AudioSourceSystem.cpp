#include "AudioSourceSystem.h"

std::weak_ptr<SystemBase> AudioSourceSystem::self;
std::vector<AudioSource> AudioSourceSystem::componentData;

#include "../Core/Application.h"
#include "../Core/AudioManager.h"
#include "Transform.h"
#include "../ECS/Entity.hpp"

#include "TransformSystem.h"

#include <FMOD/fmod.h>
#include <FMOD/fmod_common.h>

void AudioSourceSystem::OnUpdate(AudioSource* comp)
{
	if (!comp->m_bPaused)
	{
		if (comp->m_clip == nullptr || !comp->m_clip->m_bIsLoaded)
			false;

		if (comp->m_sourceTransform == nullptr || Application::GetEntitiesDeleted())
		{
			comp->m_sourceTransform = TransformSystem::GetComponent(comp->m_sEntityID);
			return;
		}

		if (comp->channel == nullptr)
		{
			FMOD_System_PlaySound(AudioManager::GetFMODSystem(), comp->m_clip->m_sound, nullptr, true, &comp->channel);

			int loopCount = comp->m_iLoopCount > 0 ? comp->m_iLoopCount - 1 : comp->m_iLoopCount;
			FMOD_Channel_SetLoopCount(comp->channel, loopCount);
		}

		FMOD_Channel_SetVolume(comp->channel, comp->m_fVolume);
		FMOD_Channel_SetPitch(comp->channel, comp->m_fPitch);
		if (comp->m_b3DSound)
		{
			FMOD_VECTOR pos;
			pos.x = comp->m_sourceTransform->m_position.x;
			pos.y = comp->m_sourceTransform->m_position.y;
			pos.z = comp->m_sourceTransform->m_position.z;

			FMOD_VECTOR vel;
			vel.x = comp->m_vVelocity.x;
			vel.y = comp->m_vVelocity.y;
			vel.z = comp->m_vVelocity.z;

			FMOD_Channel_Set3DAttributes(comp->channel, &pos, &vel);
		}


		FMOD_Channel_SetPaused(comp->channel, comp->m_bPaused);
	}
	else
	{
		if (comp->channel != nullptr)
		{
			//clean up stuff here to allow for other channels to be created.
			FMOD_Channel_Stop(comp->channel);
		}
	}
}