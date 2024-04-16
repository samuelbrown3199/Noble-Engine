#include "AudioManager.h"
#include "Logger.h"
#include "../Useful.h"

AudioManager::AudioManager()
{
	FMOD_RESULT result = FMOD_System_Create(&m_fmodSystem, FMOD_VERSION);
	if (result != FMOD_OK)
	{
		Logger::LogError("Failed to create FMOD System.", 2);
	}
	result = FMOD_System_Init(m_fmodSystem, m_iMaxChannels, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, nullptr);
	if (result != FMOD_OK)
	{
		Logger::LogError("Failed to initialise FMOD System.", 2);
	}

	FMOD_System_Set3DSettings(m_fmodSystem, 1.0f, 1.0f, 1.0f);

	Logger::LogInformation("Audio Manager initialized.");
}

AudioManager::~AudioManager()
{
	FMOD_System_Close(m_fmodSystem);
	FMOD_System_Release(m_fmodSystem);

	Logger::LogInformation("Audio Manager cleaned up");
}

void AudioManager::AddMixerOption(std::string _optionName, float _initialValue)
{
	m_mMixerOptions.insert(std::make_pair(_optionName, _initialValue));
	Logger::LogInformation(FormatString("Audio mixer option %s added.", _optionName.c_str()));
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

	Logger::LogError(FormatString("Audio Mixer option %s was not found!", _optionName), 2);
	throw std::exception();
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