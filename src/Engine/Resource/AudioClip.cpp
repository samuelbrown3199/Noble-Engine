#include "AudioClip.h"
#include "../Core/Logger.h"
#include "../Useful.h"

#include "..\Core\AudioManager.h"

AudioClip::AudioClip()
{
	//want to be default in the end, but this is useful for testing before I do the asset management rework.
	m_mode = FMOD_DEFAULT | FMOD_3D | FMOD_LOOP_NORMAL;
	m_sound = nullptr;
}

AudioClip::AudioClip(FMOD_MODE _mode)
{
	m_mode = _mode;
	m_sound = nullptr;
}

AudioClip::~AudioClip()
{
	//Need to check if this leaks memory. Hopefully not...
	FMOD_Sound_Release(m_sound);
}

void AudioClip::OnLoad()
{
	FMOD_RESULT result = FMOD_System_CreateSound(AudioManager::GetFMODSystem(), m_sResourcePath.c_str(), m_mode, nullptr, &m_sound);
	if (result != FMOD_OK)
		Logger::LogError("Failed to load sound file " + m_sResourcePath, 2);
}
