#pragma once
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"

class Model
{
public:
	Model(const std::string& path);
	void Draw(Shader& shader);
private:
	std::vector<Mesh> meshes;
	std::vector<std::string> loadedTextures;
	std::string directory;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture2D> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

