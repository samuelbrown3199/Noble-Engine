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
	glm::vec3 m_vVelocity = {0,0,0};
	bool m_bPlay = false;

	std::string m_sMixerOption;

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"clipPath", m_clip->m_sLocalPath}, {"loop", m_bLoop}, {"pitch", m_fPitch}, {"gain", m_fGain}, {"velocity", {m_vVelocity.x, m_vVelocity.y, m_vVelocity.z }}, {"play", m_bPlay}, {"mixerOption", m_sMixerOption}};
		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		m_clip = ResourceManager::LoadResource<AudioClip>(j["clipPath"]);
		m_bLoop = j["loop"];
		m_fPitch = j["pitch"];
		m_fGain = j["gain"];
		m_vVelocity = glm::vec3(j["velocity"][0], j["velocity"][1], j["velocity"][2]);
		m_bPlay = j["play"];
		m_sMixerOption = j["mixerOption"];
	}

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