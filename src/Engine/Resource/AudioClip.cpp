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

AudioClip::AudioClip(FMOD_MODE _mode)
{
	m_resourceType = "AudioClip";

	m_mode = _mode;
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