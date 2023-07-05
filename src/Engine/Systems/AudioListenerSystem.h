#include "../ECS/System.hpp"
#include "AudioListener.h"

struct AudioListenerSystem : public System<AudioListener>
{
	int m_iCurrentListener = 0;

	FMOD_VECTOR pos;
	FMOD_VECTOR vel;
	FMOD_VECTOR forward;
	FMOD_VECTOR up;

	void PreUpdate();
	void OnUpdate(AudioListener* comp);
};