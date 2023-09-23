#include "Raycaster.h"

#include "../Core/Graphics/Renderer.h"
#include "../ECS/Renderable.hpp"
#include "../Core/InputManager.h"

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>

std::deque<Ray> Raycaster::m_vRays;

Ray Raycaster::GetRayToInDirection(const glm::vec3& origin, const glm::vec3& direction)
{
    Ray ray;

    ray.m_rayOrigin = origin;
    ray.m_rayDirection = direction;

    Logger::LogInformation("Raycasting!");

    std::vector<Renderable*>* screenObjects = Renderer::GetOnScreenObjects();
    for (int i = 0; i < screenObjects->size(); i++)
    {
        int vert = 0;
        while (vert < screenObjects->at(i)->m_vertices->size())
        {
            glm::vec2 baryPos;
            float hitDis;

            glm::vec3 pos1 = screenObjects->at(i)->m_transform->m_transformMat * glm::vec4(screenObjects->at(i)->m_vertices->at(vert).pos, 1);
            glm::vec3 pos2 = screenObjects->at(i)->m_transform->m_transformMat * glm::vec4(screenObjects->at(i)->m_vertices->at(vert + 1).pos, 1);
            glm::vec3 pos3 = screenObjects->at(i)->m_transform->m_transformMat * glm::vec4(screenObjects->at(i)->m_vertices->at(vert + 2).pos, 1);

            bool hit = glm::intersectRayTriangle(ray.m_rayOrigin, ray.m_rayDirection, pos1, pos2, pos3, baryPos, hitDis);

            if (hit)
            {
                ray.m_hitObject = true;
                ray.m_hitPosition = ray.m_rayOrigin + hitDis * ray.m_rayDirection;
                ray.m_hitDistance = hitDis;
                ray.m_hitEntity = Application::GetEntity(screenObjects->at(i)->m_sEntityID);

                break;
            }

            vert += 3;
        }

        if (ray.m_hitObject)
            break;
    }

    m_vRays.push_back(ray);
    return ray;
}

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
    while(m_vRays.size() > 100)
        m_vRays.pop_front();
}