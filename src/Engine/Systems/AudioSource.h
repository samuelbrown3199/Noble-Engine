#pragma once
#ifndef AUDIOSOURCE_H_
#define AUDIOSOURCE_H_

#include "../ECS/Component.hpp"
#include "../Resource/AudioClip.h"
#include "Transform.h"
#include "../Core/ResourceManager.h"

#include "../Core/Application.h"

#include <AL/al.h>

struct AudioSource : public ComponentData<AudioSource>
{
	bool m_bHasSourceID = false;
	ALuint m_iSourceID;
	Transform* m_sourceTransform = nullptr;
	std::shared_ptr<AudioClip> m_clip;
	bool m_bLoop = false;
	float m_fPitch = 1;
	float m_fGain = 1;
	glm::vec3 m_vVelocity;
	bool m_bPlay = false;

	std::string m_sMixerOption;

	void OnInitialize(std::string _audioClipLocation, std::string _mixerOption)
	{
		m_clip = ResourceManager::LoadResource<AudioClip>(_audioClipLocation);
		m_sMixerOption = _mixerOption;
	}
	void OnInitialize(std::string _audioClipLocation, std::string _mixerOption, bool _playOnStart)
	{
		m_clip = ResourceManager::LoadResource<AudioClip>(_audioClipLocation);
		m_sMixerOption = _mixerOption;
		m_bPlay = _playOnStart;
	}
	void OnInitialize(std::string _audioClipLocation, std::string _mixerOption, bool _playOnStart, float _volume, float _pitchValue, bool _looping)
	{
		m_clip = ResourceManager::LoadResource<AudioClip>(_audioClipLocation);
		m_sMixerOption = _mixerOption;
		m_bPlay = _playOnStart;
		m_fGain = _volume;
		m_fPitch = _pitchValue;
		m_bLoop = _looping;
	}
};

#endif