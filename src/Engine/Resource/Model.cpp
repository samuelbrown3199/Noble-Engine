#include "Model.h"
#include "../Core/Logger.h"

#include "../Core/ResourceManager.h"

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

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            if (vertex.pos.x < xMin)
                xMin = vertex.pos.x;
            else if (vertex.pos.x > xMax)
                xMax = vertex.pos.x;

            if (vertex.pos.z < zMin)
                zMin = vertex.pos.z;
            else if (vertex.pos.z > zMax)
                zMax = vertex.pos.z;

            if (vertex.pos.y < yMin)
                yMin = vertex.pos.y;
            else if (vertex.pos.y > yMax)
                yMax = vertex.pos.y;

            vertex.texCoord = 
            {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.normal = 
            {
                attrib.normals[3 * index.normal_index + 0],
                attrib.normals[3 * index.normal_index + 1],
                attrib.normals[3 * index.normal_index + 2]
            };


            if (uniqueVertices.count(vertex) == 0) {
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


    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normals attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    m_bIsLoaded = true;
}

void Model::OnUnload()
{
    if (!m_bIsLoaded)
        return;

    glDeleteVertexArrays(1, &m_vaoID);
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