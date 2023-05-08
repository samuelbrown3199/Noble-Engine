#pragma once
#ifndef MESHRENDERER_H_
#define MESHRENDERER_H_

#include "../ECS/Component.hpp"
#include "../Resource/Model.h"
#include "../Resource/Texture.h"

#include "Transform.h"

struct MeshRenderer : public ComponentData<MeshRenderer>
{
	std::shared_ptr<Model> m_model;
	std::shared_ptr<Texture> m_texture;

	glm::vec4 m_colour;

	Transform* m_transform = nullptr;
};

#endif