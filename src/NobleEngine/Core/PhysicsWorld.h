#pragma once
#ifndef PHYSICSWORLD_H_
#define PHYSICSWORLD_H_

#include <memory>
#include <mutex>

#include <Bullet/btBulletDynamicsCommon.h>
#include <Bullet/btBulletCollisionCommon.h>

namespace NobleCore
{
	/**
	*Stores various options regarding the bullet physics. Created by the engine on startup.
	*/
	struct PhysicsWorld
	{
	private:

		std::mutex physicsMutex;

		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamicWorld;

		btAlignedObjectArray<btCollisionShape*> collisionShapes;

	public:
		/**
		*Creates a physics world object.
		*/
		static std::shared_ptr<PhysicsWorld> CreatePhysicsWorld();
		/**
		*Cleans up the physics world memory.
		*/
		void CleanupPhysicsWorld();
		/**
		*Adds a collider to the list of all colliders.
		*/
		void AddCollider(btCollisionShape* _shape);
		/**
		*Adds a rigidbody to the physics world.
		*/
		void AddRigidbody(btRigidBody* _body);
		/**
		*Removes a rigidbody from the physics world.
		*/
		void RemoveRigidbody(btRigidBody* _body);
		/**
		*Steps the physics simulation by the amount.
		*/
		void StepSimulation(double _amount);
	};
}

#endif