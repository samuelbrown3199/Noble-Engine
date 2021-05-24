
#include <iostream>
#include <string>
#include <map>

#include <AL/al.h>
#include <AL/alc.h>

namespace NobleCore
{
	class AudioManager
	{
	private:
		/**
		*Stores a pointer to the current audio device.
		*/
		static ALCdevice* audioDevice;
		/**
		*Stores a pointer to the current audio context.
		*/
		static ALCcontext* audioContext;
		/**
		*A check that can be used to determine whether the audio systems initialized correctly.
		*/
		static bool useAudio;
		/**
		*Stores the options for the audio mixer.
		*/
		static std::map<std::string, float> mixerOptions;

	public:
		/**
		*Constructor used to initialize the Audio Systems within the engine.
		*/
		AudioManager();
		/**
		*Adds an option to the audio mixer.
		*/
		static void AddMixerOption(std::string _optionName, float _initialValue);
		/**
		*Gets an option from the audio mixer.
		*/
		static float GetAudioMixerOption(std::string _optionName);
	};
}