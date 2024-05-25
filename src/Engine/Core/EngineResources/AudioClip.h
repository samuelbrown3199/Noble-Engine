#pragma once

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
	
	bool m_bLoop = false;
	bool m_b3DSound = false;
	FMOD_MODE m_mode;
	FMOD_SOUND* m_sound;
	/**
	*Loads a sound file and buffers it.
	*/
	void OnLoad() override;
	void OnUnload() override;

	virtual void AddResource(std::string path) override;

	virtual void SetResourceToDefaults(std::shared_ptr<Resource> res) override;

	virtual void DoResourceInterface() override;
	nlohmann::json AddToDatabase() override;
	std::shared_ptr<Resource> LoadFromJson(const std::string& path, const nlohmann::json& data) override;
	void SetDefaults(const nlohmann::json& data) override;
};