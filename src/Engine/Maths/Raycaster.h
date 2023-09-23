#pragma once

#include <glm/glm.hpp>

#include "../ECS/Entity.hpp"

struct Ray
{
	bool m_hitObject = false;

	Entity* m_hitEntity;
	glm::vec3 m_hitPosition;

	glm::vec3 m_rayOrigin;
	glm::vec3 m_rayDirection;
};

class Raycaster
{
public:

	static Ray GetRayToMousePosition(const glm::vec3& origin);

	void SetDrawRays(bool val) { m_bDrawRays = val; }
	static void DrawRays();

private:

	const int m_iRayLifetime;
	bool m_bDrawRays;

	static std::vector<Ray> m_vRays;
};