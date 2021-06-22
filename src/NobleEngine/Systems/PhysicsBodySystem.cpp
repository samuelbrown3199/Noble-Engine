#include "PhysicsBodySystem.h"

namespace NobleCore
{
	SetupComponent(NobleComponents::PhysicsBody, PhysicsBodySystem);

	void PhysicsBodySystem::OnUpdate(NobleComponents::PhysicsBody* comp)
	{
		if (!comp->rigidBody)
		{
			btVector3 localInertia(0, 0, 0);
			bool isDynamic = (comp->mass != 0.f);
			if (isDynamic)
			{
				comp->shape->calculateLocalInertia(comp->mass, localInertia);
			}

			btDefaultMotionState* motionState = new btDefaultMotionState(comp->bodyTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(comp->mass, motionState, comp->shape, localInertia);
			rbInfo.m_linearDamping = .2f;
			rbInfo.m_angularDamping = .2f;
			rbInfo.m_restitution = 0.35;

			comp->rigidBody = new btRigidBody(rbInfo);
			comp->rigidBody->setFriction(.5f);
			comp->rigidBody->setRollingFriction(.5f);

			NobleCore::Application::physicsWorld->AddRigidbody(comp->rigidBody);
		}

		if (comp->rigidBody && comp->rigidBody->getMotionState())
		{
			comp->rigidBody->getMotionState()->getWorldTransform(comp->bodyTransform);
		}

		if (comp->bodyTransform == comp->oldBodyPos)
		{
			return;
		}
		btScalar matrix[16];
		comp->bodyTransform.getOpenGLMatrix(matrix);
		comp->model = glm::mat4(1.0f);

		comp->model = glm::make_mat4(matrix);
		comp->model = glm::scale(comp->model, comp->scale);

		comp->oldBodyPos = comp->bodyTransform;
	}
}