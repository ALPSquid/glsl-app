#pragma once
#include "glew.h"
#include "glm/glm.hpp"
#include <vector>
#include <assimp/types.h>


/**
* Struct representing a 3D vertex.
*/
struct Vertex {
	// Position of the vertex in world space.
	glm::vec3 position;
	// Direction of the vertex normal.
	glm::vec3 normal;
	// Vertex texture coordinates.
	glm::vec2 texCoords;
	// Vertex tangent. (bitangent is ignored since it's just the cross of the tangent and the normal).
	glm::vec3 tangent;

	Vertex() {}
	Vertex(float x, float y, float z) {
		position = glm::vec3(x, y, z);
	}
	Vertex(glm::vec3 pos) {
		position = pos;
	}
};

struct Material {
	glm::vec3 diffuse = glm::vec3(1); // Diffuse colour.
	glm::vec3 specular = glm::vec3(1); // Specular colour.
	float shininess = 12; // How shiny the object is.
};

struct Texture {
	GLuint id;
	const char* type;
	aiString path;
	unsigned char* data;
};

class Mesh {
public:
	// Vertices that may have been rotated.
	std::vector<Vertex> vertices;
	// Array of vertex indices that make up each triangle.
	std::vector<GLuint> triangleElements;
	std::vector<Texture> textures;
	// Diffuse and specular maps take priority over material diffuse and specular settings.
	Material material;

	float boundingRadius;

protected:
	// Vertex Array, Vertex Buffer and Element Buffer.
	GLuint VAO, VBO, EBO;
	// Orginal vertices before any rotation.
	std::vector<Vertex> originVertices;

public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, float boundingRadius);
	~Mesh();

	void render(GLuint shaderProgram);

	inline bool hasTextures() { return textures.size() > 0; };

private:
	void setupMesh();
};