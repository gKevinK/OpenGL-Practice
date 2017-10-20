#ifndef MODEL_H
#define MODEL_H

#include <glm\glm.hpp>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
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
		// TODO
		return Mesh(vertices, indices, textures);
	}

	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture> textures;
		// TODO
		return textures;
	}
};

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
	unsigned int textureId;
	// TODO
	return textureId;
}

#endif // !MODEL_H
