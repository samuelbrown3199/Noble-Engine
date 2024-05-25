#include "AudioClip.h"
#include "../../Core/Logger.h"
#include "../../Useful.h"

#include "../../Core/Application.h"
#include "../../Core/AudioManager.h"
#include "../../Core/ResourceManager.h"
#include "../../Core/ToolUI.h"

AudioClip::AudioClip()
{
	m_resourceType = "AudioClip";

	//want to be default in the end, but this is useful for testing before I do the asset management rework.
	m_mode = FMOD_DEFAULT;
	m_sound = nullptr;
}

AudioClip::~AudioClip()
{
	if(m_bIsLoaded)
		OnUnload();
}

void AudioClip::OnLoad()
{
	Resource::OnLoad();

	AudioManager* aManager = Application::GetApplication()->GetAudioManager();

	FMOD_RESULT result = FMOD_System_CreateSound(aManager->GetFMODSystem(), m_sResourcePath.c_str(), m_mode, nullptr, &m_sound);
	if (result != FMOD_OK)
		LogFatalError("Failed to load sound file " + m_sResourcePath);

	m_bIsLoaded = true;
}

void AudioClip::OnUnload()
{
	//Need to check if this leaks memory. Hopefully not...
	FMOD_Sound_Release(m_sound);
	m_bIsLoaded = false;
}

void AudioClip::AddResource(std::string path)
{
	ResourceManager* resourceManager = Application::GetApplication()->GetResourceManager();
	resourceManager->AddNewResource<AudioClip>(path);
}

void AudioClip::SetResourceToDefaults(std::shared_ptr<Resource> res)
{
	std::shared_ptr<AudioClip> dyRes = std::dynamic_pointer_cast<AudioClip>(res);
	dyRes->m_mode = m_mode;
}

void AudioClip::DoResourceInterface()
{
	ImGui::Text(m_sLocalPath.c_str());

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	static NobleCheckbox loopCheckbox;
	loopCheckbox.m_pResource = this;
	loopCheckbox.DoCheckbox("Loop", m_bInitializeInterface, &m_bLoop);

	static NobleCheckbox spatialSoundCheckbox;
	spatialSoundCheckbox.m_pResource = this;
	spatialSoundCheckbox.DoCheckbox("3D Sound", m_bInitializeInterface, &m_b3DSound);

	m_mode = FMOD_DEFAULT;
	if (m_bLoop)
		m_mode |= FMOD_LOOP_NORMAL;

	if (m_b3DSound)
		m_mode |= FMOD_3D;

	m_bInitializeInterface = false;
}

nlohmann::json AudioClip::AddToDatabase()
{
	nlohmann::json data;

	data["Loop"] = m_bLoop;
	data["3DSound"] = m_b3DSound;
	data["SoundMode"] = m_mode;

	return data;
}

std::shared_ptr<Resource> AudioClip::LoadFromJson(const std::string& path, const nlohmann::json& data)
{
	std::shared_ptr<AudioClip> res = std::make_shared<AudioClip>();

	res->m_sLocalPath = path;
	res->m_sResourcePath = GetGameFolder() + path;
	if(data.find("Loop") != data.end())
	{
		res->m_bLoop = data["Loop"];
	}
	if(data.find("3DSound") != data.end())
	{
		res->m_b3DSound = data["3DSound"];
	}
	res->m_mode = data["SoundMode"];

	return res;
}

void AudioClip::SetDefaults(const nlohmann::json& data)
{
	m_mode = data["SoundMode"];
}