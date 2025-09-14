#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <assimp/material.h>

#include "mesh.h"
#include "texture.h"

class aiNode;
class aiScene;
class aiMesh;
class aiMaterial;

class Shader;

class Model
{
public:
    Model(const std::string& Filepath);
    ~Model(){};

    void Draw(Shader& InShader);

private:
    void LoadModel(const std::string& Filepath);
    // 从材质中获取纹理
    std::vector<std::shared_ptr<Texture>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type);

    void ProcessNode(aiNode *Node, const aiScene *Scene);
    Mesh ProcessMesh(aiMesh* Mesh, const aiScene* Scene);

    // 加载完成后存储到这个数组中
    std::vector<Mesh> Meshes;

    // 该模型文件所在的目录(不以/结尾)
    std::string directory;
    // 优化纹理加载，已经加载过的纹理不重复加载
    std::unordered_map<std::string, std::shared_ptr<Texture>> LoadedTextures;
};
