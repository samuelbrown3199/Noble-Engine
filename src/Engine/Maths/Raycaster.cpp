#include "Raycaster.h"

#include "../Core/Graphics/Renderer.h"
#include "../ECS/Renderable.hpp"
#include "../Core/InputManager.h"

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>

std::deque<Ray> Raycaster::m_vRays;

Ray Raycaster::GetRayToInDirection(const glm::vec3& origin, const glm::vec3& direction, float maxDistance)
{
    Ray ray;

    ray.m_rayOrigin = origin;
    ray.m_rayDirection = direction;

    std::vector<Renderable*>* screenObjects = Renderer::GetOnScreenObjects();
    for (int i = 0; i < screenObjects->size(); i++)
    {
        int vert = 0;
        float closestHit = maxDistance;
        while (vert < screenObjects->at(i)->m_indices->size()) // probably some smart ways to optimise this, checking every triangle is probably not necessary.
        {
            glm::vec2 baryPos;
            float hitDis;

            Transform* transform = NobleRegistry::GetComponent<Transform>(screenObjects->at(i)->m_transformIndex);
            if (transform == nullptr)
                continue;

            glm::vec3 pos1 = transform->m_transformMat * glm::vec4(screenObjects->at(i)->m_vertices->at(screenObjects->at(i)->m_indices->at(vert)).pos, 1);
            glm::vec3 pos2 = transform->m_transformMat * glm::vec4(screenObjects->at(i)->m_vertices->at(screenObjects->at(i)->m_indices->at(vert+1)).pos, 1);
            glm::vec3 pos3 = transform->m_transformMat * glm::vec4(screenObjects->at(i)->m_vertices->at(screenObjects->at(i)->m_indices->at(vert+2)).pos, 1);

            bool hit = glm::intersectRayTriangle(ray.m_rayOrigin, ray.m_rayDirection, pos1, pos2, pos3, baryPos, hitDis);

            if (hit && hitDis < closestHit)
            {
                ray.m_hitObject = true;
                ray.m_hitPosition = ray.m_rayOrigin + hitDis * ray.m_rayDirection;
                ray.m_hitDistance = hitDis;
                closestHit = hitDis;
                ray.m_hitEntity = Application::GetEntity(Application::GetEntityIndex(screenObjects->at(i)->m_sEntityID));
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