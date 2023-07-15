#include "Model.h"
#include "../Core/Logger.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <unordered_map>

Model::~Model()
{
    vkDestroyBuffer(Renderer::GetLogicalDevice(), m_indexBuffer, nullptr);
    vkFreeMemory(Renderer::GetLogicalDevice(), m_indexBufferMemory, nullptr);
    vkDestroyBuffer(Renderer::GetLogicalDevice(), m_vertexBuffer, nullptr);
    vkFreeMemory(Renderer::GetLogicalDevice(), m_vertexBufferMemory, nullptr);
}

void Model::OnLoad()
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, m_sResourcePath.c_str()))
    {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = 
            {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
                m_vertices.push_back(vertex);
            }

            m_indices.push_back(uniqueVertices[vertex]);
        }
    }

    Renderer::CreateVertexBuffer(m_vertexBuffer, m_vertexBufferMemory, m_vertices);
    Renderer::CreateIndexBuffer(m_indexBuffer, m_indexBufferMemory, m_indices);
}