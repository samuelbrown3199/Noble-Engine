#pragma once

#include <glm/glm.hpp>

#include "../Core/ECS/Entity.h"

struct Ray
{
	bool m_hitObject = false;

	Entity* m_hitEntity = nullptr;
	glm::vec3 m_hitPosition = glm::vec3();
	float m_hitDistance = 0.0f;

	glm::vec3 m_rayOrigin = glm::vec3();
	glm::vec3 m_rayDirection = glm::vec3();
};

class Raycaster
{
public:

	static Ray GetRayToInDirection(const glm::vec3& origin, const glm::vec3& direction, const float maxDistance = 1000.0f);
	static Ray GetRayToMousePosition(const glm::vec2 windowSize, const glm::vec2 mousePos, const float maxDistance = 1000.0f);

	void SetDrawRays(bool val) { m_bDrawRays = val; }
	static void DrawRays();

private:

	bool m_bDrawRays = false;
	static std::deque<Ray> m_vRays;
};