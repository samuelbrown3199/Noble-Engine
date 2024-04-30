#include "AudioSource.h"

#include "../Application.h"
#include "../CommandTypes.h"
#include "../ECS/Entity.h"

void AudioSource::OnUpdate()
{
	if (!m_bPaused)
	{
		if (m_clip == nullptr || !m_clip->m_bIsLoaded)
			return;

		NobleRegistry* registry = Application::GetApplication()->GetRegistry();
		AudioManager* aManager = Application::GetApplication()->GetAudioManager();

		if (m_transformIndex == -1)
		{
			m_transformIndex = registry->GetComponentIndex<Transform>(m_sEntityID);
			return;
		}

		Transform* transform = registry->GetComponent<Transform>(m_transformIndex);
		if (transform == nullptr)
			return;

		if (Application::GetApplication()->GetPlayMode() == false)
		{
			FMOD_Channel_Stop(channel);
			channel = nullptr;

			return;
		}

		if (channel == nullptr)
		{
			FMOD_System_PlaySound(aManager->GetFMODSystem(), m_clip->m_sound, nullptr, true, &channel);

			int loopCount = m_iLoopCount > 0 ? m_iLoopCount - 1 : m_iLoopCount;
			FMOD_Channel_SetLoopCount(channel, loopCount);
		}

		float volumeMixerValue = aManager->GetAudioMixerOption(m_sMixerOption);
		if (m_sMixerOption != "Master")
			volumeMixerValue *= aManager->GetAudioMixerOption("Master");

		FMOD_Channel_SetVolume(channel, m_fVolume * volumeMixerValue);
		FMOD_Channel_SetPitch(channel, m_fPitch);
		if (m_b3DSound)
		{
			FMOD_VECTOR pos;
			pos.x = transform->m_position.x;
			pos.y = transform->m_position.y;
			pos.z = transform->m_position.z;

			FMOD_VECTOR vel;
			vel.x = m_velocity.x;
			vel.y = m_velocity.y;
			vel.z = m_velocity.z;

			FMOD_Channel_Set3DAttributes(channel, &pos, &vel);
		}


		FMOD_Channel_SetPaused(channel, m_bPaused);
	}
	else
	{
		if (channel != nullptr)
		{
			//clean up stuff here to allow for other channels to be created.
			FMOD_Channel_Stop(channel);
			channel = nullptr;
		}
	}
}

void AudioSource::DoComponentInterface()
{
	ResourceManager* rManager = Application::GetApplication()->GetResourceManager();
	AudioManager* aManager = Application::GetApplication()->GetAudioManager();

	ChangeAudioClip(rManager->DoResourceSelectInterface<AudioClip>("Audio Clip", m_clip != nullptr ? m_clip->m_sLocalPath : "none"));

	static NobleDragFloat velocityDrag;
	velocityDrag.m_pComponent = this;
	velocityDrag.DoDragFloat3("Velocity", m_bInitializeInterface, &m_velocity, 0.1f);

	static NobleDragInt loopCountDrag;
	loopCountDrag.m_pComponent = this;
	loopCountDrag.DoDragInt("Loop Count", m_bInitializeInterface, &m_iLoopCount, 1, -1, 50);

	static NobleDragFloat pitchDrag;
	pitchDrag.m_pComponent = this;
	pitchDrag.DoDragFloat("Pitch", m_bInitializeInterface, &m_fPitch, 0.1f, 0.0f, 3.0f);

	static NobleDragFloat volumeDrag;
	volumeDrag.m_pComponent = this;
	volumeDrag.DoDragFloat("Volume", m_bInitializeInterface, &m_fVolume, 0.1f, 0.0f, 10.0f);

	bool paused = m_bPaused;
	if (ImGui::Checkbox("Paused", &paused))
	{
		ChangeValueCommand<bool>* command = new ChangeValueCommand<bool>(&m_bPaused, paused);
		Application::GetApplication()->PushCommand(command);
	}
	bool sound3D = m_b3DSound;
	if (ImGui::Checkbox("3D Sound", &sound3D))
	{
		ChangeValueCommand<bool>* command = new ChangeValueCommand<bool>(&m_b3DSound, sound3D);
		Application::GetApplication()->PushCommand(command);
	}

	std::map<std::string, float> mixerOptions = aManager->GetMixerOptions();
	if (ImGui::BeginListBox("Mixer Option"))
	{
		std::map<std::string, float>::iterator itr;
		for (itr = mixerOptions.begin(); itr != mixerOptions.end(); itr++)
		{
			const bool is_selected = (m_sMixerOption == itr->first);
			if(ImGui::Selectable(itr->first.c_str(), is_selected))
			{
				ImGui::SetItemDefaultFocus();
				ChangeValueCommand<std::string>* command = new ChangeValueCommand<std::string>(&m_sMixerOption, itr->first);
				Application::GetApplication()->PushCommand(command);
			}
		}
		ImGui::EndListBox();
	}

	m_bInitializeInterface = false;
}