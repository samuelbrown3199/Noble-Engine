#ifndef CAMERA_H_
#define CAMERA_H_

#include "../ECS/Component.hpp"
#include "Transform.hpp"

#include <glm/glm.hpp>

namespace NobleComponents
{
	enum CameraMode
	{
		projection,
		orthogonal
	};

	/**
	* Camera component. Useful for handling the point of view of the user.
	*/
	struct Camera : public NobleCore::ComponentData<Camera>
	{
		bool mainCamera = false;
		/**
		* Stores the camera entities transform.
		*/
		Transform* cameraTransform = nullptr;
		/**
		* Stores the type of camera this is.
		*/
		CameraMode camMode;
		/**
		*Stores the near plane clipping limit.
		*/
		float nearPlane;
		/**
		*Stores the far plane clipping limit.
		*/
		float farPlane;
		/**
		*Stores the field of view value.
		*/
		float fieldOfView = 90;
		/**
		* Stores the matrix of the camera.
		*/
		glm::mat4 cameraMatrix;

		Camera(bool _mainCam, CameraMode _camMode)
		{
			mainCamera = _mainCam;
			camMode = _camMode;
			nearPlane = 0.1f;
			farPlane = 100.0f;
			fieldOfView = 90.0f;
		}
		Camera(bool _mainCam, CameraMode _camMode, float _nearPlane, float _farPlane, float _fov)
		{
			mainCamera = _mainCam;
			camMode = _camMode;
			nearPlane = _nearPlane;
			farPlane = _farPlane;
			fieldOfView = _fov;
		}
	};
}

#endif