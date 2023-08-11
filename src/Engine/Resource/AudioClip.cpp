#include "AudioClip.h"
#include "../Core/Logger.h"
#include "../Useful.h"

#include "..\Core\AudioManager.h"

AudioClip::AudioClip()
{
	m_resourceType = "AudioClip";

	//want to be default in the end, but this is useful for testing before I do the asset management rework.
	m_mode = FMOD_DEFAULT;
	m_sound = nullptr;
}

AudioClip::~AudioClip()
{
	if(m_bIsLoaded)
		OnUnload();
}

void AudioClip::OnLoad()
{
	FMOD_RESULT result = FMOD_System_CreateSound(AudioManager::GetFMODSystem(), m_sResourcePath.c_str(), m_mode, nullptr, &m_sound);
	if (result != FMOD_OK)
		Logger::LogError("Failed to load sound file " + m_sResourcePath, 2);

	m_bIsLoaded = true;
}

void AudioClip::OnUnload()
{
	//Need to check if this leaks memory. Hopefully not...
	FMOD_Sound_Release(m_sound);
	m_bIsLoaded = false;
}

void AudioClip::DoResourceInterface()
{
	ImGui::Text(m_sLocalPath.c_str());
	ImGui::Text(m_resourceType.c_str());
	
	if (m_bIsLoaded)
	{
		ImGui::Text("Audio Clip is currently in use and can't be modified.");
		return;
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	bool loop = m_mode & FMOD_LOOP_NORMAL;
	ImGui::Checkbox("Loop", &loop);

	bool spatialSound = m_mode & FMOD_3D;
	ImGui::Checkbox("3D Sound", &spatialSound);

	m_mode = FMOD_DEFAULT;
	if (loop)
		m_mode |= FMOD_LOOP_NORMAL;

	if (spatialSound)
		m_mode |= FMOD_3D;
}

nlohmann::json AudioClip::AddToDatabase()
{
	nlohmann::json data;

	data["SoundMode"] = m_mode;

	return data;
}

void AudioClip::LoadFromJson(std::string path, nlohmann::json data)
{
	Resource::LoadFromJson(path, data);
	m_mode = data["SoundMode"];
}