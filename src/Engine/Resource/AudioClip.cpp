#include "AudioClip.h"
#include "../Core/Logger.h"
#include "../Useful.h"

#include <stb_vorbis.h>

void AudioClip::OnLoad()
{
	alGenBuffers(1, &m_iAudioClipID);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR)
	{
		Logger::LogError(FormatString("alGenBuffers failed due to error %d", error), 1);
	}

	ALenum format = 0;
	ALsizei freq = 0;
	std::vector<char> bufferData;
	LoadOgg(m_sResourcePath, bufferData, format, freq);

	alBufferData(m_iAudioClipID, format, &bufferData.at(0), static_cast<ALsizei>(bufferData.size()), freq);
}
/**
*Loads an OGG file.
*/
void AudioClip::LoadOgg(const std::string& _fileDirectory, std::vector<char>& _buffer, ALenum& _format, ALsizei& _freq)
{
	int channels = 0;
	int sampleRate = 0;
	short* output = NULL;
	int samples = stb_vorbis_decode_filename(m_sResourcePath.c_str(), &channels, &sampleRate, &output);
	int amountOfBytesPerSample = 0;
	if (samples == -1)
	{
		throw std::exception();
	}
	// Record the format required by OpenAL
	if (channels == 1)
	{
		_format = AL_FORMAT_MONO16;
		amountOfBytesPerSample = 2;
	}
	else
	{
		_format = AL_FORMAT_STEREO16;
		amountOfBytesPerSample = 4;
	}
	// Record the sample rate required by OpenAL
	_freq = sampleRate;
 	_buffer.resize(samples * amountOfBytesPerSample);
	memcpy(&_buffer.at(0), output, _buffer.size());
	// Clean up the read data
	free(output);
}