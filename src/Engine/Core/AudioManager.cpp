#include "AudioManager.h"
#include "Logger.h"
#include "../Useful.h"

AudioManager::AudioManager()
{
	FMOD_RESULT result = FMOD_System_Create(&m_fmodSystem, FMOD_VERSION);
	if (result != FMOD_OK)
	{
		LogFatalError("Failed to create FMOD System.");
	}
	result = FMOD_System_Init(m_fmodSystem, m_iMaxChannels, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, nullptr);
	if (result != FMOD_OK)
	{
		LogFatalError("Failed to initialise FMOD System.");
	}

	FMOD_System_Set3DSettings(m_fmodSystem, 1.0f, 1.0f, 1.0f);

	LogTrace("Audio Manager initialized.");
}

AudioManager::~AudioManager()
{
	FMOD_System_Close(m_fmodSystem);
	FMOD_System_Release(m_fmodSystem);

	LogTrace("Audio Manager cleaned up");
}

void AudioManager::AddMixerOption(std::string _optionName, float _initialValue)
{
	m_mMixerOptions.insert(std::make_pair(_optionName, _initialValue));
	LogInfo(FormatString("Audio mixer option %s added.", _optionName.c_str()));
}

float AudioManager::GetAudioMixerOption(std::string _optionName)
{
	std::map<std::string, float>::iterator itr;
	for (itr = m_mMixerOptions.begin(); itr != m_mMixerOptions.end(); itr++)
	{
		if (itr->first == _optionName)
		{
			return itr->second;
		}
	}

	LogFatalError(FormatString("Audio Mixer option %s was not found!", _optionName));
	return 0.0f;
}

void AudioManager::UpdateAudioMixerOption(std::string _optionName, float _newValue)
{
	std::map<std::string, float>::iterator itr;
	for (itr = m_mMixerOptions.begin(); itr != m_mMixerOptions.end(); itr++)
	{
		if (itr->first == _optionName)
		{
			itr->second = _newValue;
		}
	}
}