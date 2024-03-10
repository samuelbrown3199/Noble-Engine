#include "Model.h"
#include "../../Core/Logger.h"


#include "../../Core/Graphics/VulkanTypes.h"
#include "../../Core/ResourceManager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <unordered_map>

Model::Model()
{
    m_resourceType = "Model";
}

Model::~Model()
{
    if (m_bIsLoaded)
        OnUnload();
}

void Model::OnLoad()
{
    if (m_bIsLoaded)
        return;

    Resource::OnLoad();

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, m_sResourcePath.c_str()))
    {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    float xMin = 0, xMax = 0, yMin = 0, yMax = 0, zMin = 0, zMax = 0;

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};

            vertex.m_position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            if (vertex.m_position.x < xMin)
                xMin = vertex.m_position.x;
            else if (vertex.m_position.x > xMax)
                xMax = vertex.m_position.x;

            if (vertex.m_position.z < zMin)
                zMin = vertex.m_position.z;
            else if (vertex.m_position.z > zMax)
                zMax = vertex.m_position.z;

            if (vertex.m_position.y < yMin)
                yMin = vertex.m_position.y;
            else if (vertex.m_position.y > yMax)
                yMax = vertex.m_position.y;

            vertex.m_uvX = attrib.texcoords[2 * index.texcoord_index + 0];
            vertex.m_uvY = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1];

            vertex.m_normal = 
            {
                attrib.normals[3 * index.normal_index + 0],
                attrib.normals[3 * index.normal_index + 1],
                attrib.normals[3 * index.normal_index + 2]
            };

            vertex.m_colour =
            {
                1,
                1,
                1,
                1
            };

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
                m_vertices.push_back(vertex);
            }

            m_indices.push_back(uniqueVertices[vertex]);
        }
    }

    std::vector<glm::vec3> bbox;
    bbox.push_back(glm::vec3(xMin, yMin, zMin));
    bbox.push_back(glm::vec3(xMax, yMin, zMin));
    bbox.push_back(glm::vec3(xMin, yMax, zMax));
    bbox.push_back(glm::vec3(xMax, yMax, zMax));
    bbox.push_back(glm::vec3(xMin, yMax, zMin));
    bbox.push_back(glm::vec3(xMax, yMax, zMin));
    bbox.push_back(glm::vec3(xMin, yMin, zMax));
    bbox.push_back(glm::vec3(xMax, yMin, zMax));
    for (int i = 0; i < bbox.size(); i++)
    {
        m_modelBoundingBox.push_back(bbox[i]);
    }

    m_meshBuffer = Renderer::UploadMesh(m_indices, m_vertices);
    m_bIsLoaded = true;
}

void Model::OnUnload()
{
    if (!m_bIsLoaded)
        return;

    m_bIsLoaded = false;
}

void Model::AddResource(std::string path)
{
    ResourceManager::AddNewResource<Model>(path);
}

std::vector<std::shared_ptr<Resource>> Model::GetResourcesOfType()
{
    return ResourceManager::GetAllResourcesOfType<Model>();
}

std::shared_ptr<Resource> Model::LoadFromJson(const std::string& path, const nlohmann::json& data)
{
    std::shared_ptr<Model> res = std::make_shared<Model>();

    res->m_sLocalPath = path;
    res->m_sResourcePath = GetGameFolder() + path;

    return res;
}