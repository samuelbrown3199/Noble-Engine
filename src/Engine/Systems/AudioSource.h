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

	static std::vector<AudioSource> m_componentData;

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

	virtual void DoComponentInterface() override
	{
		ImGui::Text(m_sEntityID.c_str());
		ImGui::Dummy(ImVec2(0.0f, 20.0f));

		ImGui::Text("Something to select clip (WIP)");
		ImGui::Text("Function built into Res Manager?");

		ImGui::DragInt("Loop Count", &m_iLoopCount, 1, -1, 50);
		ImGui::DragFloat("Pitch", &m_fPitch, 0.1, 0, 3);
		ImGui::DragFloat("Volume", &m_fVolume, 0.1, 0, 10);
		ImGui::Checkbox("Paused", &m_bPaused);
		ImGui::Checkbox("3D Sound", &m_b3DSound);
	}

	Component* GetAsComponent(std::string entityID) override
	{
		return GetComponent(entityID);
	}

	virtual void AddComponent() override;
	virtual void RemoveComponent(std::string entityID) override;
	virtual void RemoveAllComponents() override;

	AudioSource* GetComponent(std::string entityID);

	virtual void Update(bool useThreads, int maxComponentsPerThread) override;
	virtual void ThreadUpdate(int _buffer, int _amount) override;
	virtual void OnUpdate() override;

	virtual void Render(bool useThreads, int maxComponentsPerThread) override;
	virtual void ThreadRender(int _buffer, int _amount) override;

	virtual void LoadComponentDataFromJson(nlohmann::json& j) override;
	virtual nlohmann::json WriteComponentDataToJson() override;
};