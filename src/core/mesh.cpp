#include <og/core/mesh.h>

#include <format>
#include <memory>
#include <string>

#include <og/core/shader.h>
#include <og/core/texture.h>
#include <og/core/vertex_array.h>
#include <og/core/vertex_buffer.h>
#include <og/core/vertex_buffer_layout.h>

Mesh::Mesh(const std::vector<Vertex>& InVertices, const std::vector<uint32_t>& InIndices,
           const std::vector<std::shared_ptr<Texture>>& InDiffuseTextures,
           const std::vector<std::shared_ptr<Texture>>& InSpecularTextures) :
    Vertices(InVertices),
    Indices(InIndices),
    DiffuseTextures(InDiffuseTextures),
    SpecularTextures(InSpecularTextures)
{
    SetupMesh();
}

void Mesh::Draw(Shader& InShader)
{
    int diffuseTexNum  = DiffuseTextures.size();
    int specularTexNum = SpecularTextures.size();

    int slot = 0;
    for (int i = 0; i < diffuseTexNum; i++)
    {
        std::string texName = std::format("uTextureDiffuse{}", i);
        InShader.SetUniform1i(texName, slot++);
    }

    for (int i = 0; i < specularTexNum; i++)
    {
        std::string texName = std::format("uTextureSpecular{}", i);
        InShader.SetUniform1i(texName, slot++);
    }

    VAO->Bind();
    InShader.Bind();
    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, Indices.data());
}

void Mesh::SetupMesh()
{
    VAO = std::make_unique<VertexArray>();

    std::shared_ptr<VertexBuffer> vertexBuffer =
            std::make_shared<VertexBuffer>(Vertices.data(), Vertices.size() * sizeof(Vertex));
    VertexBufferLayout layout;
    layout.PushAttribute<float>(3); // position
    layout.PushAttribute<float>(3); // normal
    layout.PushAttribute<float>(2); // texcoords

    VAO->AddBuffer(vertexBuffer, layout);

    // 绑定所有纹理
    unsigned int slot = 0;
    for (int i = 0; i < DiffuseTextures.size(); i++)
    {
        std::shared_ptr<Texture> diffuseTex = DiffuseTextures[i];
        diffuseTex->Bind(slot++);
    }

    for (int i = 0; i < SpecularTextures.size(); i++)
    {
        std::shared_ptr<Texture> specTex = SpecularTextures[i];
        specTex->Bind(slot++);
    }
}
