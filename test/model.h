#ifndef MODEL_H
#define MODEL_H

#include <glm\glm.hpp>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <stb_image.h>
#include "mesh.h"
#include "shader.h"
#include <iostream>
#include <vector>

class Model
{
public:
	std::vector<Texture> Textures_Loaded;
	std::vector<Mesh> Meshes;
	std::string Directory;
	bool GammaCorrection;

	Model(const char *path) { loadModel(path); };

	void Draw(unsigned int shader) {
		for (unsigned int i = 0; i < Meshes.size(); i++)
			Meshes[i].Draw(shader);
	}

private:

	void loadModel(const std::string &path)
	{
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
			return;
		}
		Directory = path.substr(0, path.find_last_of('\\'));
		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode *node, const aiScene *scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			Meshes.push_back(processMesh(mesh, scene));
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			if (mesh->mTextureCoords[0]) {
				vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			} else {
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}
			vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
			vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
			vertices.push_back(vertex);
		}
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		return Mesh(vertices, indices, textures);
	}

	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int j = 0; j < Textures_Loaded.size(); j++) {
				if (std::strcmp(Textures_Loaded[j].path.C_Str(), str.C_Str()) == 0) {
					textures.push_back(Textures_Loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip) {
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->Directory);
				texture.type = typeName;
				texture.path = str;
				textures.push_back(texture);
				Textures_Loaded.push_back(texture);
			}
		}
		return textures;
	}
};

unsigned int TextureFromFile(const std::string &name, const std::string &directory, bool gamma = false)
{
	std::string path = directory + '\\' + name;
	unsigned int textureId;
	glGenTextures(1, &textureId);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(data);
	} else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	return textureId;
}

#endif // !MODEL_H
