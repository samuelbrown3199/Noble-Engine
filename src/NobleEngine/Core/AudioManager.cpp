#include "AudioManager.h"

namespace NobleCore
{
	ALCdevice* AudioManager::audioDevice;
	ALCcontext* AudioManager::audioContext;
	bool AudioManager::useAudio = true;
	std::map<std::string, float> AudioManager::mixerOptions;

	AudioManager::AudioManager()
	{
		audioDevice = alcOpenDevice(NULL);
		if (!audioDevice)
		{
			std::cout << "Application failed to initialize audio device!" << std::endl;
			useAudio = false;
		}
		audioContext = alcCreateContext(audioDevice, NULL);
		if (!audioContext)
		{
			std::cout << "Application failed to initialize audio context!" << std::endl;
			alcCloseDevice(audioDevice);
			useAudio = false;
		}
		if (!alcMakeContextCurrent(audioContext))
		{
			std::cout << "Application failed to assign audio context!" << std::endl;
			alcDestroyContext(audioContext);
			alcCloseDevice(audioDevice);
			useAudio = false;
		}

		AddMixerOption("master", 1.0f);
	}

	void AudioManager::AddMixerOption(std::string _optionName, float _initialValue)
	{
		mixerOptions.insert(std::make_pair(_optionName, _initialValue));
	}

	float AudioManager::GetAudioMixerOption(std::string _optionName)
	{
		std::map<std::string, float>::iterator itr;
		for (itr = mixerOptions.begin(); itr != mixerOptions.end(); itr++)
		{
			if (itr->first == _optionName)
			{
				return itr->second;
			}
		}

		std::cout << "Audio Mixer option " << _optionName << " was not found!" << std::endl;
		throw std::exception();
	}
}