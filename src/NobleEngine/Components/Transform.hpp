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

		glm::vec3 oldPosition;
		glm::vec3 oldRotation;
		glm::vec3 oldScale;

		/**
		*Default initializer for transform. Initializes position and rotation to 0, and scale to 1.
		*/
		void OnInitialize()
		{
			position = glm::vec3(0, 0, 0);
			rotation = glm::vec3(0, 0, 0);
			scale = glm::vec3(1, 1, 1);
		}
		/**
		*Main initializer for transform.
		*/
		void OnInitialize(const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _sca)
		{
			position = _pos;
			rotation = _rot;
			scale = _sca;
		}
		/**
		*Main initializer for transform. Initializes scale to 1.
		*/
		void OnInitialize(const glm::vec3& _pos, const glm::vec3& _rot)
		{
			position = _pos;
			rotation = _rot;
		}
		/**
		*Main initializer for transform. Initializes rotation to 0, scale to 1.
		*/
		void OnInitialize(const glm::vec3& _pos)
		{
			position = _pos;
		}
	};
}

#endif