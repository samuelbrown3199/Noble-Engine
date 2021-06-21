#ifndef MESH_H_
#define MESH_H_

#include "../ECS/Component.hpp"
#include "../ResourceManagement/Model.hpp"
#include "../ResourceManagement/ShaderProgram.hpp"
#include "../ResourceManagement/Texture.h"

#include "StaticTransform.hpp"
#include "Transform.hpp"

namespace NobleComponents
{
	enum MeshTransformMode
	{
		Static,
		NonStatic
	};

	struct Mesh : public NobleCore::ComponentData<Mesh>
	{
		glm::mat4* modelMatrix;
		std::shared_ptr<NobleResources::Model> model;
		std::shared_ptr<NobleResources::ShaderProgram> shader;
		std::shared_ptr<NobleResources::Texture> texture;

		MeshTransformMode transformMode;
		Transform* meshTransform = nullptr;
		StaticTransform* meshStaticTransform = nullptr;

		void OnInitialize(MeshTransformMode _transformMode, std::shared_ptr<NobleResources::Model> _model, std::shared_ptr<NobleResources::ShaderProgram> _shader, std::shared_ptr<NobleResources::Texture> _texture)
		{
			transformMode = _transformMode;
			model = _model;
			shader = _shader;
			texture = _texture;
		}
	};
}

#endif