#pragma once
#ifndef PHYSICSBODY_H_
#define PHYSICSBODY_H_

#include "../Core/Application.h"
#include "../Core/PhysicsWorld.h"
#include "../ECS/Component.hpp"
#include "../ECS/Entity.hpp"

#include <iostream>

#include <Bullet/btBulletDynamicsCommon.h>

namespace NobleComponents
{

	/**
	*The shape options for the collider.
	*/
	enum ColliderShape
	{
		box,
		sphere,
		cylinder
	};

	/**
	*This component stores the physics properties of an entity.
	*/
	struct PhysicsBody : public NobleCore::ComponentData<PhysicsBody> 
	{
	public:

		/**
		*Used to detemine the shape of the collider.
		*/
		ColliderShape colliderShape;
		/**
		*Stores a collider shape pointer.
		*/
		btCollisionShape* shape;
		/**
		*Stores the rigidbody transform within the physics simulation.
		*/
		btTransform bodyTransform;
		btTransform oldBodyPos;
		/**
		*Stores the rigidbody mass.
		*/
		btScalar mass;
		/**
		*Stores a pointer to the rigidbody.
		*/
		btRigidBody* rigidBody = nullptr;

		glm::vec3 scale;
		/**
		* Stores the physics body model matrix.
		*/
		glm::mat4 model;

		/**
		* Initializes the body with the shape, mass and transform information provided.
		*/
		void OnInitialize(ColliderShape _shape, btScalar _mass, glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _sca)
		{
			colliderShape = _shape;
			scale = _sca;

			switch (colliderShape)
			{
			case box:
				shape = new btBoxShape(btVector3(_sca.x, _sca.y, _sca.z));
				break;
			case sphere:
				shape = new btSphereShape(_sca.x);
				break;
			case cylinder:
				shape = new btCylinderShape(btVector3(_sca.x, _sca.y, _sca.z));
				break;
			}
			NobleCore::Application::physicsWorld->AddCollider(shape);

			bodyTransform.setOrigin(btVector3(_pos.x, _pos.y, _pos.z));
			bodyTransform.setRotation(btQuaternion(_rot.x, _rot.y, _rot.z));

			mass = _mass;
		}

		void OnRemove()
		{
			NobleCore::Application::physicsWorld->RemoveRigidbody(rigidBody);
			delete rigidBody;
			delete shape;
		}
	};
}

#endif