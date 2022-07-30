#include "AudioManager.h"
#include "Logger.h"
#include "../Useful.h"

ALCdevice* AudioManager::m_audioDevice;
ALCcontext* AudioManager::m_audioContext;
bool AudioManager::m_bUseAudio = true;
std::map<std::string, float> AudioManager::m_mMixerOptions;

Camera* AudioManager::m_camera;

AudioManager::AudioManager()
{
	m_audioDevice = alcOpenDevice(NULL);
	if (!m_audioDevice)
	{
		Logger::LogError("Application failed to initialize audio device!", 2);
		m_bUseAudio = false;
	}
	m_audioContext = alcCreateContext(m_audioDevice, NULL);
	if (!m_audioContext)
	{
		Logger::LogError("Application failed to initialize audio context!", 2);
		alcCloseDevice(m_audioDevice);
		m_bUseAudio = false;
	}
	if (!alcMakeContextCurrent(m_audioContext))
	{
		Logger::LogError("Application failed to assign audio context!", 2);
		alcDestroyContext(m_audioContext);
		alcCloseDevice(m_audioDevice);
		m_bUseAudio = false;
	}

	m_camera = Renderer::GetCamera();

	Logger::LogInformation("Audio manager initialized");
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

void AudioManager::UpdateListenerPos()
{
	alListener3f(AL_POSITION, m_camera->m_position.x, m_camera->m_position.y, m_camera->m_position.z);
}

void AudioManager::GetOpenALError()
{
	ALenum error = alGetError();
	if (error != AL_NO_ERROR)
	{
		Logger::LogError(FormatString("OpenAL error %d", error), 1);
	}
}