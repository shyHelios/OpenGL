#include <og/core/model.h>

#include <iostream>
#include <memory>

#include <assimp/scene.h> // 必须放在Importer之前，否则会报链接错误
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <og/core/mesh.h>

Model::Model(const std::string& Filepath)
{
    LoadModel(Filepath);
}

void Model::Draw(Shader& InShader)
{
    for (int i = 0; i < Meshes.size(); i++)
    {
        Meshes[i].Draw(InShader);
    }
}

void Model::LoadModel(const std::string& Filepath)
{
    Assimp::Importer importer;
    // 后处理，将所有图元转换为三角形，将纹理的UV反转
    // aiProcess_GenNormals：如果模型不包含法向量的话，就为每个顶点创建法线。
    // aiProcess_SplitLargeMeshes：将比较大的网格分割成更小的子网格，如果你的渲染有最大顶点数限制，只能渲染较小的网格，那么它会非常有用。
    // aiProcess_OptimizeMeshes：和上个选项相反，它会将多个小网格拼接为一个大的网格，减少绘制调用从而进行优化。
    const aiScene* scene = importer.ReadFile(Filepath, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Assimp导入模型失败: " << importer.GetErrorString() << std::endl;
        return;
    }
    directory = Filepath.substr(0, Filepath.find_last_of('/'));
    ProcessNode(scene->mRootNode, scene);
}

std::vector<std::shared_ptr<Texture>> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
    int numTextures = mat->GetTextureCount(type);
    std::vector<std::shared_ptr<Texture>> textures;
    textures.reserve(numTextures);
    for (int i = 0; i < numTextures; i++)
    {
        aiString name;
        mat->GetTexture(type, i, &name);
        std::string filepath = directory + "/" + std::string(name.C_Str());
        if (LoadedTextures.find(filepath) == LoadedTextures.end())
        {
            std::shared_ptr<Texture> tex = std::make_shared<Texture>(filepath);
            textures.emplace_back(tex);
            LoadedTextures[filepath] = tex;
        }
        else
        {
            textures.emplace_back(LoadedTextures[filepath]);
        }
    }
    return textures;
}

void Model::ProcessNode(aiNode* Node, const aiScene* Scene)
{
    // 处理节点的所有网格
    for (int i = 0; i < Node->mNumMeshes; i++)
    {
        aiMesh* Mesh = Scene->mMeshes[Node->mMeshes[i]];
        Meshes.push_back(ProcessMesh(Mesh, Scene));
    }

    // 对所有子节点重复这一过程
    for (int i = 0; i < Node->mNumChildren; i++)
    {
        ProcessNode(Node->mChildren[i], Scene);
    }
}

Mesh Model::ProcessMesh(aiMesh* InMesh, const aiScene* Scene)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<std::shared_ptr<Texture>> diffuseTextures;
    std::vector<std::shared_ptr<Texture>> specularTextures;

    // 位置、法向、纹理坐标
    for (int i = 0; i < InMesh->mNumVertices; i++)
    {
        // Assimp 把顶点位置数组叫做mVertices
        glm::vec3 position;
        position.x = InMesh->mVertices[i].x;
        position.y = InMesh->mVertices[i].y;
        position.z = InMesh->mVertices[i].z;

        glm::vec3 normal;
        normal.x = InMesh->mNormals[i].x;
        normal.y = InMesh->mNormals[i].y;
        normal.z = InMesh->mNormals[i].z;

        // 最多有8套UV坐标，我们只用第一套
        glm::vec2 texCoords;
        if (InMesh->mTextureCoords[0])
        {
            texCoords.x = InMesh->mTextureCoords[0][i].x;
            texCoords.y = InMesh->mTextureCoords[0][i].y;
        }
        else
        {
            texCoords = glm::vec2(0.f);
        }

        vertices.emplace_back(position, normal, texCoords);
    }

    // 记录索引
    for (int i = 0; i < InMesh->mNumFaces; i++)
    {
        aiFace face    = InMesh->mFaces[i];
        int numIndices = face.mNumIndices;
        for (int i = 0; i < numIndices; i++)
        {
            indices.push_back(face.mIndices[i]);
        }
    }

    // 一个mesh只使用一个材质，如果导入的一个mesh使用多个材质，Assimp会自动分割
    if (InMesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = Scene->mMaterials[InMesh->mMaterialIndex];
        diffuseTextures      = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
        specularTextures     = LoadMaterialTextures(material, aiTextureType_SPECULAR);
    }
    return Mesh(vertices, indices, std::move(diffuseTextures), std::move(specularTextures));
}
