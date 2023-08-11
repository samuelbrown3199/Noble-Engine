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
	AudioClip();
	~AudioClip();
	

	FMOD_MODE m_mode;
	FMOD_SOUND* m_sound;
	/**
	*Loads a sound file and buffers it.
	*/
	void OnLoad() override;
	void OnUnload() override;

	virtual void AddResource(std::string path) override;
	virtual std::vector<std::shared_ptr<Resource>> GetResourcesOfType() override;

	virtual void DoResourceInterface() override;
	nlohmann::json AddToDatabase() override;
	void LoadFromJson(std::string path, nlohmann::json data) override;
};

#endif