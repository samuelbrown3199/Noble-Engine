#pragma once

#include "../ECS/Component.h"
#include "../EngineResources/AudioClip.h"
#include "Transform.h"
#include "../ResourceManager.h"

#include "../Application.h"

#include <FMOD/fmod.h>
#include <FMOD/fmod_common.h>

struct AudioSource : public Component
{
	FMOD_CHANNEL* channel = nullptr;
	int m_transformIndex = -1;

	std::string m_sTargetClipPath = "none";
	std::shared_ptr<AudioClip> m_clip = nullptr;

	int m_iLoopCount = 0;
	float m_fPitch = 1;
	float m_fVolume = 1;
	glm::vec3 m_velocity = { 0,0,0 };
	bool m_bPaused = false;
	bool m_b3DSound = false;

	int m_iMixerOption;

	AudioSource& operator=(const AudioSource& other)
	{
		m_transformIndex = -1;

		m_sTargetClipPath = other.m_sTargetClipPath;
		m_clip = other.m_clip;

		m_iLoopCount = other.m_iLoopCount;
		m_fPitch = other.m_fPitch;
		m_fVolume = other.m_fVolume;
		m_velocity = other.m_velocity;
		m_bPaused = other.m_bPaused;
		m_b3DSound = other.m_b3DSound;
		m_iMixerOption = other.m_iMixerOption;

		return *this;
	}

	~AudioSource()
	{
		if (channel != nullptr)
		{
			FMOD_Channel_Stop(channel);
		}
	}

	std::string GetComponentID() override
	{
		return "AudioSource";
	}

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"clipPath", m_sTargetClipPath}, {"loopCount", m_iLoopCount}, {"pitch", m_fPitch}, {"volume", m_fVolume}, {"velocity", {m_velocity.x, m_velocity.y, m_velocity.z }}, {"paused", m_bPaused}, {"3DSound", m_b3DSound},{"mixerOption", m_iMixerOption} };
		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		ResourceManager* rManager = Application::GetApplication()->GetResourceManager();

		m_sTargetClipPath = j["clipPath"];
		m_iLoopCount = j["loopCount"];
		m_fPitch = j["pitch"];
		m_fVolume = j["volume"];
		m_velocity = glm::vec3(j["velocity"][0], j["velocity"][1], j["velocity"][2]);
		m_bPaused = j["paused"];
		m_b3DSound = j["3DSound"];
		m_iMixerOption = j["mixerOption"];
	}

	void OnInitialize()
	{

	}

	void OnRemove() override
	{
		m_transformIndex = -1;
	}

	float GetEffectiveVolume();

	virtual void DoComponentInterface() override;
	virtual void OnUpdate() override;
};