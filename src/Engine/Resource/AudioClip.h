#pragma once
#ifndef AUDIOCLIP_H_
#define AUDIOCLIP_H_

#include "Resource.h"

#include <FMOD/fmod.h>
#include <FMOD/fmod_common.h>


/**
*Stores an audio file for use in FMOD.
*/
struct AudioClip : public Resource
{
	~AudioClip();

	FMOD_SOUND* m_sound;
	/**
	*Loads a sound file and buffers it.
	*/
	void OnLoad();
};

#endif