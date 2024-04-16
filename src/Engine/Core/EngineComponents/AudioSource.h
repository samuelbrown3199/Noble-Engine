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
	std::shared_ptr<AudioClip> m_clip = nullptr;
	int m_iLoopCount = 0;
	float m_fPitch = 1;
	float m_fVolume = 1;
	glm::vec3 m_velocity = { 0,0,0 };
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

	std::string GetComponentID() override
	{
		return "AudioSource";
	}

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"clipPath", m_clip->m_sLocalPath}, {"loopCount", m_iLoopCount}, {"pitch", m_fPitch}, {"volume", m_fVolume}, {"velocity", {m_velocity.x, m_velocity.y, m_velocity.z }}, {"paused", m_bPaused}, {"3DSound", m_b3DSound},{"mixerOption", m_sMixerOption} };
		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		ResourceManager* rManager = Application::GetApplication()->GetResourceManager();

		m_clip = rManager->LoadResource<AudioClip>(j["clipPath"]);
		m_iLoopCount = j["loopCount"];
		m_fPitch = j["pitch"];
		m_fVolume = j["volume"];
		m_velocity = glm::vec3(j["velocity"][0], j["velocity"][1], j["velocity"][2]);
		m_bPaused = j["paused"];
		m_b3DSound = j["3DSound"];
		m_sMixerOption = j["mixerOption"];
	}

	void OnInitialize()
	{

	}

	void OnRemove() override
	{
		m_transformIndex = -1;
	}

	void ChangeAudioClip(std::shared_ptr<AudioClip> clip)
	{
		if (clip == nullptr)
			return;

		if (m_clip != nullptr && clip->m_sLocalPath == m_clip->m_sLocalPath)
			return;

		FMOD_Channel_Stop(channel);
		channel = nullptr;

		m_clip = clip;
	}

	virtual void DoComponentInterface() override
	{
		ResourceManager* rManager = Application::GetApplication()->GetResourceManager();
		AudioManager* aManager = Application::GetApplication()->GetAudioManager();

		ChangeAudioClip(rManager->DoResourceSelectInterface<AudioClip>("Audio Clip", m_clip != nullptr ? m_clip->m_sLocalPath : "none"));

		float vel[3] = { m_velocity.x,m_velocity.y ,m_velocity.z };
		ImGui::DragFloat3("Velocity", vel);
		m_velocity = glm::vec3(vel[0], vel[1], vel[2]);

		ImGui::DragInt("Loop Count", &m_iLoopCount, 1.0f, -1.0f, 50.0f);
		ImGui::DragFloat("Pitch", &m_fPitch, 0.1f, 0.0f, 3.0f);
		ImGui::DragFloat("Volume", &m_fVolume, 0.1f, 0.0f, 10.0f);
		ImGui::Checkbox("Paused", &m_bPaused);
		ImGui::Checkbox("3D Sound", &m_b3DSound);

		std::map<std::string, float> mixerOptions = aManager->GetMixerOptions();
		if (ImGui::BeginListBox("Mixer Option"))
		{
			std::map<std::string, float>::iterator itr;
			for (itr = mixerOptions.begin(); itr != mixerOptions.end(); itr++)
			{
				const bool is_selected = (m_sMixerOption == itr->first);
				ImGui::Selectable(itr->first.c_str(), is_selected);

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
					m_sMixerOption = itr->first;
				}
			}
			ImGui::EndListBox();
		}
	}

	virtual void OnUpdate() override;
};