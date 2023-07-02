#include "AudioClip.h"
#include "../Core/Logger.h"
#include "../Useful.h"

#include "..\Core\AudioManager.h"

AudioClip::~AudioClip()
{
	//Need to check if this leaks memory. Hopefully not...
	FMOD_Sound_Release(m_sound);
}

void AudioClip::OnLoad()
{
	FMOD_RESULT result = FMOD_System_CreateSound(AudioManager::GetFMODSystem(), m_sResourcePath.c_str(), FMOD_DEFAULT | FMOD_LOOP_NORMAL, nullptr, &m_sound); //currently using FMOD_DEFAULT, will want to load sounds as 2D or 3D here at somepoint.
	if (result != FMOD_OK)
		Logger::LogError("Failed to load sound file " + m_sResourcePath, 2);
}
