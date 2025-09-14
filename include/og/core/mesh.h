#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "vertex_array.h"

class VertexBuffer;
class Texture;
class Shader;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh
{
public:
    Mesh(const std::vector<Vertex>& InVertices, const std::vector<uint32_t>& InIndices,
         const std::vector<std::shared_ptr<Texture>>& InDiffuseTextures,
         const std::vector<std::shared_ptr<Texture>>& InSpecularTextures);

    void Draw(Shader& InShader);

private:
    void SetupMesh();

    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;
    // 一个model可能含有多个mesh，每个mesh都可能引用同一张纹理，因此用shared_ptr
    std::vector<std::shared_ptr<Texture>> DiffuseTextures;
    std::vector<std::shared_ptr<Texture>> SpecularTextures;

    std::unique_ptr<VertexArray> VAO;
};
