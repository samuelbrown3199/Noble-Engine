#pragma once

#include "../ECS/Component.hpp"
#include "../Resource/AudioClip.h"
#include "Transform.h"
#include "../Core/ResourceManager.h"

#include "../Core/Application.h"

#include <FMOD/fmod.h>
#include <FMOD/fmod_common.h>

struct AudioSource : public Component
{
	FMOD_CHANNEL* channel = nullptr;
	Transform* m_sourceTransform = nullptr;
	std::shared_ptr<AudioClip> m_clip = nullptr;
	int m_iLoopCount = 0;
	float m_fPitch = 1;
	float m_fVolume = 1;
	glm::vec3 m_vVelocity = { 0,0,0 };
	bool m_bPaused = false;
	bool m_b3DSound = false;

	std::string m_sMixerOption;

	~AudioSource()
	{
		if (channel != nullptr)
		{
			FMOD_Channel_Stop(channel);
		}
	}

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"clipPath", m_clip->m_sLocalPath}, {"loopCount", m_iLoopCount}, {"pitch", m_fPitch}, {"volume", m_fVolume}, {"velocity", {m_vVelocity.x, m_vVelocity.y, m_vVelocity.z }}, {"paused", m_bPaused}, {"3DSound", m_b3DSound},{"mixerOption", m_sMixerOption} };
		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		m_clip = ResourceManager::LoadResource<AudioClip>(j["clipPath"]);
		m_iLoopCount = j["loopCount"];
		m_fPitch = j["pitch"];
		m_fVolume = j["volume"];
		m_vVelocity = glm::vec3(j["velocity"][0], j["velocity"][1], j["velocity"][2]);
		m_bPaused = j["paused"];
		m_b3DSound = j["3DSound"];
		m_sMixerOption = j["mixerOption"];
	}

	void OnInitialize(std::string _audioClipLocation, std::string _mixerOption)
	{
		m_clip = ResourceManager::LoadResource<AudioClip>(_audioClipLocation);
		m_sMixerOption = _mixerOption;
	}
	void OnInitialize(std::string _audioClipLocation, std::string _mixerOption, bool _paused)
	{
		m_clip = ResourceManager::LoadResource<AudioClip>(_audioClipLocation);
		m_sMixerOption = _mixerOption;
		m_bPaused = _paused;
	}
	void OnInitialize(std::string _audioClipLocation, std::string _mixerOption, bool _paused, float _volume, float _pitchValue, int _loopCount)
	{
		m_clip = ResourceManager::LoadResource<AudioClip>(_audioClipLocation);
		m_sMixerOption = _mixerOption;
		m_bPaused = _paused;
		m_fVolume = _volume;
		m_fPitch = _pitchValue;
		m_iLoopCount = _loopCount;
	}
};