#include "../Core/Application.h"
#include "MeshSystem.h"

namespace NobleCore
{
	SetupComponent(NobleComponents::Mesh, MeshSystem);

	void MeshSystem::PreRender()
	{
		projectionViewMatrix = glm::mat4(1.0f);
		projectionViewMatrix = Application::mainCam->projectionMatrix * Application::mainCam->viewMatrix;
	}

	void MeshSystem::OnRender(NobleComponents::Mesh* comp)
	{
		comp->modelMatrix = &Application::GetEntity(comp->entityID)->GetComponent<NobleComponents::Transform>()->model; //doesnt work for static transform.

		if (comp->model)
		{
			glm::mat4 projectionViewModel = glm::mat4(1.0f);
			projectionViewModel = projectionViewMatrix * *comp->modelMatrix;

			comp->shader->UseProgram();
			comp->shader->BindVector3("colour", glm::vec3(1.0, 0.0, 1.0));
			comp->shader->BindMat4("u_ProjectionViewModel", projectionViewModel);

			glBindVertexArray(comp->model->vaoID);
			glDrawArrays(GL_TRIANGLES, 0, comp->model->drawCount);
			glBindVertexArray(0);
			glUseProgram(0);

		}
	}
}