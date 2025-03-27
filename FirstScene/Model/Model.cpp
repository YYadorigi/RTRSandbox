#include <iostream>
#include <glad/glad.h>
#include "Model.h"

Model::Model(const std::string& path, bool flipY) : flipY(flipY)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);

	loadedTextures.clear();
}

Model::Model(Model&& other) noexcept
{
	meshes = std::move(other.meshes);
	loadedTextures = std::move(other.loadedTextures);
	directory = std::move(other.directory);
	flipY = other.flipY;
	other.flipY = false;
}

Model& Model::operator=(Model&& other) noexcept
{
	if (this != &other) {
		meshes = std::move(other.meshes);
		loadedTextures = std::move(other.loadedTextures);
		directory = std::move(other.directory);
		flipY = other.flipY;
		other.flipY = false;
	}
	return *this;
}

void Model::draw(const Shader& shader)
{
	for (auto& mesh : meshes) {
		mesh.draw(shader);
	}
}

void Model::drawInstanced(const Shader& shader, const std::vector<glm::vec3>& translations)
{
	for (auto& mesh : meshes) {
		mesh.drawInstanced(shader, translations);
	}
}

void Model::processNode(const aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.emplace_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(const aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::shared_ptr<TextureMap2D>> textures;
	float shininess = 0.0f;
	float opacity = 1.0f;

	bool hasPositions = mesh->HasPositions();
	bool hasNormals = mesh->HasNormals();
	bool hasTexCoords = mesh->HasTextureCoords(0);
	bool hasFaces = mesh->HasFaces();
	bool hasMaterial = mesh->mMaterialIndex >= 0;

	if (!hasPositions || !hasFaces) {
		std::cerr << "Mesh does not have positions or faces" << std::endl;
		return Mesh({}, {}, {}, shininess, opacity);
	}
	if (!hasNormals) {
		std::cerr << "Smooth normals not generated" << std::endl;
		return Mesh({}, {}, {}, shininess, opacity);
	}

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex{};

		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (hasTexCoords) {
			vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else {
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}
		vertices.emplace_back(std::move(vertex));
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.emplace_back(face.mIndices[j]);
		}
	}

	if (hasMaterial) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<std::shared_ptr<TextureMap2D>> ambientMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "ambient");
		std::vector<std::shared_ptr<TextureMap2D>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
		std::vector<std::shared_ptr<TextureMap2D>> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		material->Get(AI_MATKEY_SHININESS, shininess);
		material->Get(AI_MATKEY_OPACITY, opacity);
	}

	return Mesh(vertices, indices, textures, shininess, opacity);
}

std::vector<std::shared_ptr<TextureMap2D>> Model::loadMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& typeName)
{
	std::vector<std::shared_ptr<TextureMap2D>> textures;
	bool sRGB = (type == aiTextureType_AMBIENT || type == aiTextureType_DIFFUSE || type == aiTextureType_SPECULAR);

	for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
		aiString str;
		material->GetTexture(type, i, &str);
		std::string name = str.C_Str();

		bool skip = false;
		for (const auto& loadedTexture : loadedTextures) {
			if (loadedTexture->getName() == name) {
				textures.emplace_back(loadedTexture);
				skip = true;
				break;
			}
		}

		if (!skip) {
			std::string path = directory + '/' + name;
			std::shared_ptr<TextureMap2D> tex = std::make_shared<TextureMap2D>(
				path,
				name,
				typeName,
				0,
				GL_CLAMP_TO_EDGE,
				GL_CLAMP_TO_EDGE,
				GL_LINEAR_MIPMAP_LINEAR,
				GL_LINEAR,
				sRGB,
				flipY
			);
			loadedTextures.emplace_back(tex);
			textures.emplace_back(tex);
		}
	}

	return textures;
}