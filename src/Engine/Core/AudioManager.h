#pragma once
#ifndef AUDIOMANAGER_H_
#define AUDIOMANAGER_H_

#include <string>
#include <map>

#include <FMOD/fmod.h>
#include <FMOD/fmod_common.h>

class AudioManager
{
private:

	const int m_iMaxChannels = 64;
	static FMOD_SYSTEM* m_fmodSystem;
	/**
	*A check that can be used to determine whether the audio systems initialized correctly.
	*/
	static bool m_bUseAudio;
	/**
	*Stores the options for the audio mixer.
	*/
	static std::map<std::string, float> m_mMixerOptions;

public:
	/**
	*Constructor used to initialize the Audio Systems within the engine.
	*/
	AudioManager();
	~AudioManager();
	/**
	*Adds an option to the audio mixer.
	*/
	static void AddMixerOption(std::string _optionName, float _initialValue);
	/**
	*Gets an option from the audio mixer.
	*/
	static float GetAudioMixerOption(std::string _optionName);
	static void UpdateAudioMixerOption(std::string _optionName, float _newValue);

	static FMOD_SYSTEM* GetFMODSystem() { return m_fmodSystem; }
	static void UpdateSystem() { FMOD_System_Update(m_fmodSystem); }
};

#endif