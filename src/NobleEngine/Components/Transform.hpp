#pragma once
#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include "../ECS/Component.hpp"

#include <glm/glm.hpp>

namespace NobleComponents
{
	/**
	*Transform component, stores position rotation and scale.
	*/
	struct Transform : public NobleCore::ComponentData<Transform>
	{
	public:

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
		*Default constructor for transform. Initializes position and rotation to 0, and scale to 1.
		*/
		Transform()
		{
			position = glm::vec3(0, 0, 0);
			rotation = glm::vec3(0, 0, 0);
			scale = glm::vec3(1, 1, 1);
		}
		/**
		*Main constructor for transform.
		*/
		Transform(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale)
		{
			position = _position;
			rotation = _rotation;
			scale = _scale;
		}
		/**
		*Main constructor for transform. Initializes scale to 1.
		*/
		Transform(glm::vec3 _position, glm::vec3 _rotation)
		{
			position = _position;
			rotation = _rotation;
			scale = glm::vec3(1,1,1);
		}
		/**
		*Main constructor for transform. Initializes rotation to 0, scale to 1.
		*/
		Transform(glm::vec3 _position)
		{
			position = _position;
			rotation = glm::vec3(0, 0, 0);
			scale = glm::vec3(1, 1, 1);
		}
	};
}

#endif