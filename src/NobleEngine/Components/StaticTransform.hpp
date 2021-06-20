#pragma once
#ifndef STATICTRANSFORM_H_
#define STATICTRANSFORM_H_

#include "../ECS/Component.hpp"

#include <glm/glm.hpp>

namespace NobleComponents
{
	/**
	*Static Transform component, stores position rotation and scale, but is not allowed to move.
	*/
	struct StaticTransform : public NobleCore::ComponentData<StaticTransform>
	{
		/**
		*Used to determine whether the matrix has been generated.
		*/
		bool dirty = false;
		/**
		* Stores the model matrix of the transform component.
		*/
		glm::mat4 modelMat;
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

		/**
		*Default constructor for static transform. Initializes position and rotation to 0, and scale to 1.
		*/
		void OnInitialize()
		{
			position = glm::vec3(0, 0, 0);
			rotation = glm::vec3(0, 0, 0);
			scale = glm::vec3(1, 1, 1);
		}
		/**
		*Main constructor for static transform.
		*/
		void OnInitialize(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale)
		{
			position = _position;
			rotation = _rotation;
			scale = _scale;
		}
	};
}

#endif