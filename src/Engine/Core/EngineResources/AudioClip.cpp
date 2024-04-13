#include "AudioClip.h"
#include "../../Core/Logger.h"
#include "../../Useful.h"

#include "../../Core/Application.h"
#include "../../Core/AudioManager.h"
#include "../../Core/ResourceManager.h"

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

	FMOD_RESULT result = FMOD_System_CreateSound(AudioManager::GetFMODSystem(), m_sResourcePath.c_str(), m_mode, nullptr, &m_sound);
	if (result != FMOD_OK)
		Logger::LogError("Failed to load sound file " + m_sResourcePath, 2);

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

std::vector<std::shared_ptr<Resource>> AudioClip::GetResourcesOfType()
{
	ResourceManager* resourceManager = Application::GetApplication()->GetResourceManager();
	return resourceManager->GetAllResourcesOfType<AudioClip>();
}

void AudioClip::SetResourceToDefaults(std::shared_ptr<Resource> res)
{
	std::shared_ptr<AudioClip> dyRes = std::dynamic_pointer_cast<AudioClip>(res);
	dyRes->m_mode = m_mode;
}

void AudioClip::DoResourceInterface()
{
	ImGui::Text(m_sLocalPath.c_str());
	ImGui::Text(m_resourceType.c_str());
	
	if (m_bIsLoaded)
	{
		ImGui::Text("Audio Clip is currently in use and can't be modified.");
		return;
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	bool loop = m_mode & FMOD_LOOP_NORMAL;
	ImGui::Checkbox("Loop", &loop);

	bool spatialSound = m_mode & FMOD_3D;
	ImGui::Checkbox("3D Sound", &spatialSound);

	m_mode = FMOD_DEFAULT;
	if (loop)
		m_mode |= FMOD_LOOP_NORMAL;

	if (spatialSound)
		m_mode |= FMOD_3D;
}

nlohmann::json AudioClip::AddToDatabase()
{
	nlohmann::json data;

	data["SoundMode"] = m_mode;

	return data;
}

std::shared_ptr<Resource> AudioClip::LoadFromJson(const std::string& path, const nlohmann::json& data)
{
	std::shared_ptr<AudioClip> res = std::make_shared<AudioClip>();

	res->m_sLocalPath = path;
	res->m_sResourcePath = GetGameFolder() + path;
	res->m_mode = data["SoundMode"];

	return res;


}

void AudioClip::SetDefaults(const nlohmann::json& data)
{
	m_mode = data["SoundMode"];
}