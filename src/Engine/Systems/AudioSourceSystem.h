#include "../ECS/System.hpp"
#include "AudioSource.h"

struct AudioSourceSystem : public System<AudioSource>
{
	void OnUpdate(AudioSource* comp);
};