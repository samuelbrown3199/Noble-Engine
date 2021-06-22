#include "../Core/Application.h"
#include "MeshSystem.h"

namespace NobleCore
{
	SetupComponent(NobleComponents::Mesh, MeshSystem);

	void MeshSystem::GetTransformInformation(NobleComponents::Mesh* comp)
	{
		switch (comp->transformMode)
		{
		case NobleComponents::MeshTransformMode::Static:
			if (comp->meshStaticTransform == NULL)
			{
				comp->meshStaticTransform = Application::GetEntity(comp->entityID)->GetComponent<NobleComponents::StaticTransform>();
			}
			comp->modelMatrix = &comp->meshStaticTransform->model;
			break;
		case NobleComponents::MeshTransformMode::NonStatic:
			if (comp->meshTransform == NULL)
			{
				comp->meshTransform = Application::GetEntity(comp->entityID)->GetComponent<NobleComponents::Transform>();
			}
			comp->modelMatrix = &comp->meshTransform->model;
			break;
		case NobleComponents::MeshTransformMode::Physics:
			if (comp->physicsTransform == NULL)
			{
				comp->physicsTransform = Application::GetEntity(comp->entityID)->GetComponent<NobleComponents::PhysicsBody>();
			}
			comp->modelMatrix = &comp->physicsTransform->model;
			break;
		}
	}

	void MeshSystem::PreRender()
	{
		projectionViewMatrix = glm::mat4(1.0f);
		projectionViewMatrix = Application::mainCam->projectionMatrix * Application::mainCam->viewMatrix;
	}

	void MeshSystem::OnRender(NobleComponents::Mesh* comp)
	{
		GetTransformInformation(comp);

		if (comp->model)
		{
			glm::mat4 projectionViewModel = glm::mat4(1.0f);
			projectionViewModel = projectionViewMatrix * *comp->modelMatrix;

			comp->shader->UseProgram();
			comp->shader->BindInt("diffuseTexture", 0);
			comp->shader->BindMat4("u_ProjectionViewModel", projectionViewModel);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, comp->texture->textureID);

			glBindVertexArray(comp->model->vaoID);
			glDrawArrays(GL_TRIANGLES, 0, comp->model->drawCount);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glUseProgram(0);

		}
	}
}