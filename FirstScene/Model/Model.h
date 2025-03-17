#pragma once
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Texture/Texture2D.h"
#include "Mesh.h"

class Model
{
public:
	Model(const std::string& path, bool flipY = false);
	void draw(Shader& shader);
private:
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<std::shared_ptr<TextureMap2D>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
private:
	std::vector<Mesh> meshes;
	std::vector<std::shared_ptr<TextureMap2D>> loadedTextures;
	std::string directory;
	bool flipY;
};
