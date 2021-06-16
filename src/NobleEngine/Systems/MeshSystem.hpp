#include "../ECS/System.hpp"
#include "../Components/Mesh.hpp"
#include "../Components/Transform.hpp"

namespace NobleCore
{
	struct MeshSystem : public System<NobleComponents::Mesh>
	{
		void OnRender(NobleComponents::Mesh* comp)
		{
			if (!comp->transform)
			{
				comp->transform = Application::GetEntity(comp->entityID)->GetComponent<NobleComponents::Transform>();
			}

			if (comp->model)
			{
				comp->shader->UseProgram();
				comp->shader->BindModelMat(comp->transform->modelMat);
				comp->shader->BindViewMat(Application::mainCam->cameraMatrix);

				glBindVertexArray(comp->model->vaoID);
				glDrawArrays(GL_TRIANGLES, 0, comp->model->drawCount);
				glBindVertexArray(0);
				glUseProgram(0);
			}
		}
	};

	SetupComponent(NobleComponents::Mesh, MeshSystem);
}