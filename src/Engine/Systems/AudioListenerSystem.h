#include "../ECS/System.hpp"
#include "AudioListener.h"

struct AudioListenerSystem : public System<AudioListener>
{
	int m_iCurrentListener = 0;

	void PreUpdate();
	void OnUpdate(AudioListener* comp);
};