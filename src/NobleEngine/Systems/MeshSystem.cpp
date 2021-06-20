#include "../Core/Application.h"
#include "MeshSystem.h"

namespace NobleCore
{

	SetupComponent(NobleComponents::Mesh, MeshSystem);

	void MeshSystem::OnRender(NobleComponents::Mesh* comp)
	{
		if (!comp->transform) //need support for static transforms.
		{
			comp->transform = Application::GetEntity(comp->entityID)->GetComponent<NobleComponents::Transform>();
		}

		if (comp->model)
		{
			comp->shader->UseProgram();
			comp->shader->BindVector3("colour", glm::vec3(0.0, 0.0, 1.0));
			comp->shader->BindMat4("u_Model", comp->transform->modelMat);
			comp->shader->BindMat4("u_Projection", Application::mainCam->projectionMatrix);
			comp->shader->BindMat4("u_View", Application::mainCam->viewMatrix);

			glBindVertexArray(comp->model->vaoID);
			glDrawArrays(GL_TRIANGLES, 0, comp->model->drawCount);
			glBindVertexArray(0);
			glUseProgram(0);
		}
	}
}