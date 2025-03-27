#pragma once
#include <string>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Texture/Texture.h"
#include "Mesh.h"

class Model
{
public:
	Model(const std::string& path, bool flipY = false);
	~Model() = default;
	Model(const Model& other) = delete;
	Model& operator=(const Model& other) = delete;
	Model(Model&& other) noexcept;
	Model& operator=(Model&& other) noexcept;

	void draw(const Shader& shader);
	void drawInstanced(const Shader& shader, const std::vector<glm::vec3>& translations);
private:
	void processNode(const aiNode* node, const aiScene* scene);
	Mesh processMesh(const aiMesh* mesh, const aiScene* scene);
	std::vector<std::shared_ptr<TextureMap2D>> loadMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& typeName);
private:
	std::vector<Mesh> meshes;
	std::vector<std::shared_ptr<TextureMap2D>> loadedTextures;
	std::string directory;
	bool flipY;
};
