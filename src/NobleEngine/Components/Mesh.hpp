#ifndef MESH_H_
#define MESH_H_

#include "../ECS/Component.hpp"
#include "../ResourceManagement/Model.hpp"
#include "../ResourceManagement/ShaderProgram.hpp"
#include "Transform.hpp"

namespace NobleComponents
{
	struct Mesh : public NobleCore::ComponentData<Mesh>
	{
		Transform* transform;
		std::shared_ptr<NobleResources::Model> model;
		std::shared_ptr<NobleResources::ShaderProgram> shader;

		Mesh(std::shared_ptr<NobleResources::Model> _model, std::shared_ptr<NobleResources::ShaderProgram> _shader)
		{
			model = _model;
			shader = _shader;
		}
	};
}

#endif