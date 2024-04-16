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
	FMOD_SYSTEM* m_fmodSystem;
	/**
	*A check that can be used to determine whether the audio systems initialized correctly.
	*/
	bool m_bUseAudio = true;
	/**
	*Stores the options for the audio mixer.
	*/
	std::map<std::string, float> m_mMixerOptions;

public:
	/**
	*Constructor used to initialize the Audio Systems within the engine.
	*/
	AudioManager();
	~AudioManager();
	/**
	*Adds an option to the audio mixer.
	*/
	void AddMixerOption(std::string _optionName, float _initialValue);
	/**
	*Gets an option from the audio mixer.
	*/
	float GetAudioMixerOption(std::string _optionName);
	void UpdateAudioMixerOption(std::string _optionName, float _newValue);
	std::map<std::string, float> GetMixerOptions() { return m_mMixerOptions; }

	FMOD_SYSTEM* GetFMODSystem() { return m_fmodSystem; }
	void UpdateSystem() { FMOD_System_Update(m_fmodSystem); }
};

#endif