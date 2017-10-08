#include "../stdafx.h"
#include "Graphics/Model.h"
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "utils/Utils.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"



Model::Model() {}

Model::Model(GLchar* path, ImportSettings importSettings/* = ImportSettings()*/) {
	this->importSettings = importSettings;
	loadModel(path);
}

void Model::render(GLuint shaderProgram) {
	// Render meshes.
	for (auto& mesh : meshes) {
		mesh.render(shaderProgram);
	}
}

void Model::addMesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, float boundingRadius) {
	meshes.push_back(Mesh(vertices, indices, textures, boundingRadius));
}

void Model::setMaterial(glm::vec3 diffuse, glm::vec3 specular, float shininess) {
	for (auto& mesh : meshes) {
		mesh.material.diffuse = diffuse;
		mesh.material.specular = specular;
		mesh.material.shininess = shininess;
	}
}

void Model::addTexture(const char* path, const char* type) {
	Texture texture;
	texture.path = path;
	texture.type = type;
	texture.id = Utils::loadTexture(path);

	for (auto& mesh : meshes) {
		mesh.textures.push_back(texture);
	}
}

void Model::loadModel(std::string path) {
	std::cout << "--- Loading model --- \n" << path << std::endl;

	// Create Assimp importer and read the file with realtime quality processing. This triangulates the mesh, among other things.
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_Quality);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Error loading model '" << path << "': \n" << importer.GetErrorString() << std::endl;
		return;
	}

	// Get base path to this asset.
	baseDir = path.substr(0, path.find_last_of('/') + 1);
	// Process meshes.
	for (GLuint i = 0; i < scene->mNumMeshes; i++) {
		meshes.push_back(processMesh(scene->mMeshes[i], scene));
	}
	std::cout << "--- Finished loading model ---" << std::endl;
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	// Get vertex data.
	glm::vec3 furthestPoint = glm::vec3(0);
	glm::vec3 centre = glm::vec3(0);
	float boundingRadius;
	for (GLuint i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		// Position & normal.
		glm::vec3 pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		vertex.position = pos;
		vertex.normal = normal;

		// First texture coord if it exists.
		glm::vec2 texCoord;
		if (mesh->mTextureCoords[0]) {
			// Get texture coordinate, inverting the Y coordinate if set.
			texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, (importSettings.invertYCoord) ? 1 - mesh->mTextureCoords[0][i].y : mesh->mTextureCoords[0][i].y);
		}
		vertex.texCoords = texCoord;

		// Smooth tangent.
		if (mesh->HasTangentsAndBitangents()) vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

		vertices.push_back(vertex);

		// Find furthest point along each axis for bounding sphere radius.
		if (glm::abs(pos.x) > furthestPoint.x) furthestPoint.x = glm::abs(pos.x);
		if (glm::abs(pos.y) > furthestPoint.y) furthestPoint.y = glm::abs(pos.y);
		if (glm::abs(pos.z) > furthestPoint.z) furthestPoint.z = glm::abs(pos.z);
	}
	// Calculate bounding radius.
	boundingRadius = glm::sqrt(glm::dot(furthestPoint, furthestPoint));

	// Indices of vertices that make up faces.
	for (GLuint i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// Materials.
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// Diffuse maps.
		textures = loadTextures(material, aiTextureType_DIFFUSE, ShaderLoader::Vars::MAT_DIFFUSE);

		// Specular maps.
		auto specMaps = loadTextures(material, aiTextureType_SPECULAR, ShaderLoader::Vars::MAT_SPECULAR);
		textures.insert(textures.end(), specMaps.begin(), specMaps.end());

		// Normal maps. (.obj saves normals as aiTextureType_HEIGHT)
		auto normalMaps = loadTextures(material, /*aiTextureType_NORMALS*/aiTextureType_HEIGHT, ShaderLoader::Vars::MAT_NORMAL);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	std::cout << "Loaded mesh: \n  Verts: " << vertices.size() << "\n  Tris: " << (indices.size()/3) << "\n  Bounding Radius: " << boundingRadius << std::endl;
	return Mesh(vertices, indices, textures, boundingRadius);
}

std::vector<Texture> Model::loadTextures(aiMaterial* mat, aiTextureType type, const char* typeName) {
	std::vector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
		aiString path;
		mat->GetTexture(type, i, &path);

		// Only load textures that haven't already been loaded.
		bool preLoaded = false;
		for (auto& texture : textures) {
			if (path == texture.path) {
				preLoaded = true;
				break;
			}
		}

		if (!preLoaded) {
			Texture texture;
			std::string fullPath = baseDir + path.C_Str();
			texture.id = Utils::loadTexture(fullPath.c_str()); // For loading and binding to an OpenGL texture.
			texture.type = typeName;
			texture.path = path;
			textures.push_back(texture);
			loadedTextures.push_back(texture);
		}
	}

	return textures;
}
