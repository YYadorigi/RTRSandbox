#include "Model.h"

Model::Model(const std::string& path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::Draw(Shader& shader)
{
	for (auto& mesh : meshes) {
		mesh.Draw(shader);
	}
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.emplace_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture2D> textures;

	bool hasPositions = mesh->HasPositions();
	bool hasNormals = mesh->HasNormals();
	bool hasTexCoords = mesh->HasTextureCoords(0);
	bool hasFaces = mesh->HasFaces();
	bool hasMaterial = mesh->mMaterialIndex >= 0;

	if (!hasPositions || !hasFaces) {
		std::cout << "Mesh does not have positions or faces" << std::endl;
		return Mesh({}, {}, {});
	}
	if (!hasNormals) {
		std::cout << "Smooth normals not generated" << std::endl;
		return Mesh({}, {}, {});
	}

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex{};

		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (hasTexCoords) {
			vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		} else {
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}
		vertices.emplace_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.emplace_back(face.mIndices[j]);
		}
	}

	if (hasMaterial) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture2D> ambientMaps = std::move(loadMaterialTextures(material,
			aiTextureType_AMBIENT, "ambient"));
		std::vector<Texture2D> diffuseMaps = std::move(loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "diffuse"));
		std::vector<Texture2D> specularMaps = std::move(loadMaterialTextures(material,
			aiTextureType_SPECULAR, "specular"));
		for (auto& ambientMap : ambientMaps)
			textures.emplace_back(std::move(ambientMap));
		for (auto& diffuseMap : diffuseMaps)
			textures.emplace_back(std::move(diffuseMap));
		for (auto& specularMap : specularMaps)
			textures.emplace_back(std::move(specularMap));
	}

	return Mesh(vertices, indices, std::move(textures));
}

std::vector<Texture2D> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
{
	std::vector<Texture2D> textures;
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
		aiString str;
		material->GetTexture(type, i, &str);
		std::string name = str.C_Str();

		bool loaded = false;
		for (const auto& loadedTexture : loadedTextures) {
			if (loadedTexture == name) {
				loaded = true;
				break;
			}
		}

		if (!loaded) {
			std::string path = directory + '/' + name;
			textures.emplace_back(
				Texture2D(
					path.c_str(),
					GL_REPEAT, GL_REPEAT,
					GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
					0, GL_UNSIGNED_BYTE, typeName
				)
			);
			loadedTextures.emplace_back(name);
		}
	}
	return textures;
}