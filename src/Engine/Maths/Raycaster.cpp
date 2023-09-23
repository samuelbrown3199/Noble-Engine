#include "Raycaster.h"

#include "../Core/InputManager.h"

std::vector<Ray> Raycaster::m_vRays;

Ray Raycaster::GetRayToMousePosition(const glm::vec3& origin)
{
    Ray ray;

    glm::vec3 hitPoint = glm::vec3(0, 0, 0);

    glm::vec3 mousePos = glm::vec3(InputManager::m_iMouseX, InputManager::m_iMouseY, 0);
    mousePos = glm::normalize(mousePos);
    mousePos.z = -1;

    Logger::LogInformation(FormatString("Mouse Ray Direction: %.2f %.2f %.2f. Length %.2f", mousePos.x, mousePos.y, mousePos.z, glm::length(mousePos))); //Dont think this is right.

    //Implement from https://glm.g-truc.net/0.9.0/api/a00162.html. GLM has intersect functions apparently! Returns a true or false and a position!

    ray.m_rayOrigin = origin;
    ray.m_rayDirection = mousePos;

    m_vRays.push_back(ray);
    return ray;
}


void Raycaster::DrawRays()
{
    //if(m_bDrawRays)
}