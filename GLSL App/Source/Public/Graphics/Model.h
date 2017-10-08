#pragma once
#include "glew.h"
#include <vector>
#include "Mesh.h"

/** A Model is a collection of meshes. */
class Model {

public :
	/** Model import settings. */ 
	struct ImportSettings {
		bool invertYCoord = false;
	};

protected:
	std::vector<Mesh> meshes;
	std::vector<Texture> loadedTextures;
	std::string baseDir;

	/** Import settings used for the current model. */
	ImportSettings importSettings;

public:
	Model();
	Model(GLchar* path, ImportSettings importSettings = ImportSettings());

	void render(GLuint shaderProgram);

	/** Manually add a mesh to this model. */
	void addMesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, float boundingRadius);

	// Set the material for the whole model.
	void setMaterial(glm::vec3 diffuse, glm::vec3 specular, float shininess);
	/**
	* Add a texture to the whole model.
	* Parameter: const char* path  Path to the texture file.
	* Parameter: const char* type  Type of texture, see ShaderLoader::Vars::MAT_*
	*/
	void addTexture(const char* path, const char* type);

protected:	
	void loadModel(std::string path);
	Mesh processMesh(struct aiMesh* mesh, const struct aiScene* scene);
	std::vector<Texture> loadTextures(struct aiMaterial* mat, enum aiTextureType type, const char* typeName);
};
