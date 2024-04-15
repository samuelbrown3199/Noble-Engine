#include "Raycaster.h"

#include "../Core/Graphics/Renderer.h"
#include "../Core/ECS/Renderable.hpp"
#include "../Core/InputManager.h"

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>

std::deque<Ray> Raycaster::m_vRays;

Ray Raycaster::GetRayToInDirection(const glm::vec3& origin, const glm::vec3& direction, float maxDistance)
{
    Renderer* renderer = Application::GetApplication()->GetRenderer();
    NobleRegistry* registry = Application::GetApplication()->GetRegistry();

    Ray ray;
    ray.m_rayOrigin = origin;
    ray.m_rayDirection = direction;

    std::vector<Renderable*>* screenObjects = renderer->GetOnScreenObjects();
    float closestHit = maxDistance;
    for (int i = 0; i < screenObjects->size(); i++)
    {
        int vert = 0;
        while (vert < screenObjects->at(i)->m_indices->size()) // probably some smart ways to optimise this, checking every triangle is probably not necessary.
        {
            glm::vec2 baryPos;
            float hitDis;

            Transform* transform = registry->GetComponent<Transform>(screenObjects->at(i)->m_transformIndex);
            if (transform == nullptr)
                continue;

            glm::vec3 pos1 = transform->m_transformMat * glm::vec4(screenObjects->at(i)->m_vertices->at(screenObjects->at(i)->m_indices->at(vert)).m_position, 1);
            glm::vec3 pos2 = transform->m_transformMat * glm::vec4(screenObjects->at(i)->m_vertices->at(screenObjects->at(i)->m_indices->at(vert+1)).m_position, 1);
            glm::vec3 pos3 = transform->m_transformMat * glm::vec4(screenObjects->at(i)->m_vertices->at(screenObjects->at(i)->m_indices->at(vert+2)).m_position, 1);

            bool hit = glm::intersectRayTriangle(ray.m_rayOrigin, ray.m_rayDirection, pos1, pos2, pos3, baryPos, hitDis);

            if (hit && hitDis < closestHit)
            {
                ray.m_hitObject = true;
                ray.m_hitPosition = ray.m_rayOrigin + hitDis * ray.m_rayDirection;
                ray.m_hitDistance = hitDis;
                closestHit = hitDis;
                ray.m_hitEntity = Application::GetApplication()->GetEntity(Application::GetApplication()->GetEntityIndex(screenObjects->at(i)->m_sEntityID));
            }

            vert += 3;
        }
    }

    m_vRays.push_back(ray);
    return ray;
}

Ray Raycaster::GetRayToMousePosition(float maxDistance)
{
    Renderer* renderer = Application::GetApplication()->GetRenderer();
    glm::vec2 screenSize = renderer->GetScreenSize();

    // converts a position from the 2d xpos, ypos to a normalized 3d direction
    float x = (2.0f * InputManager::m_iMouseX) / screenSize.x - 1.0f;
    float y = 1.0f - (2.0f * InputManager::m_iMouseY) / screenSize.y;
    float z = 1.0f;
    glm::vec3 ray_nds = glm::vec3(x, y, z);
    glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
    // eye space to clip we would multiply by projection so
    // clip space to eye space is the inverse projection
    glm::mat4 projMatrix = renderer->GenerateProjMatrix();
    projMatrix[1][1] *= -1;

    glm::vec4 ray_eye = glm::inverse(projMatrix) * ray_clip;
    // convert point to forwards
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
    // world space to eye space is usually multiply by view so
    // eye space to world space is inverse view
    glm::vec4 inv_ray_wor = (glm::inverse(renderer->GenerateViewMatrix()) * ray_eye);
    glm::vec3 ray_wor = glm::vec3(inv_ray_wor.x, inv_ray_wor.y, inv_ray_wor.z);
    ray_wor = normalize(ray_wor);

    Transform* camTransform;
    if (renderer->GetCamera() != nullptr)
    {
        NobleRegistry* registry = Application::GetApplication()->GetRegistry();

        camTransform = registry->GetComponent<Transform>(renderer->GetCamera()->m_camTransformIndex);
        return GetRayToInDirection(camTransform->m_position, ray_wor, maxDistance);
    }

    return Ray();
}


void Raycaster::DrawRays()
{
    while(m_vRays.size() > 100)
        m_vRays.pop_front();
}