#pragma once
#ifndef STATICTRANSFORM_H_
#define STATICTRANSFORM_H_

#include "../ECS/Component.hpp"

#include <glm/glm.hpp>

namespace NobleComponents
{
	struct StaticTransform : public NobleCore::ComponentData<StaticTransform>
	{
		/**
		* Stores the model matrix of the transform component.
		*/
		glm::mat4 modelMat = glm::mat4(1.0f);
		/**
		*Stores the position of the transform.
		*/
		glm::vec3 position;
		/**
		*Stores the rotation of the transform.
		*/
		glm::vec3 rotation;
		/**
		*Stores the scale of the transform.
		*/
		glm::vec3 scale;
	};
}

#endif