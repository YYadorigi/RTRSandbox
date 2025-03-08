#pragma once
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Texture/Texture.h"
#include "Mesh.h"

class Model
{
public:
	Model(const std::string& path);
	void draw(Shader& shader);
private:
	std::vector<Mesh> meshes;
	std::vector<std::shared_ptr<Texture2D>> loadedTextures;
	std::string directory;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<std::shared_ptr<Texture2D>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
