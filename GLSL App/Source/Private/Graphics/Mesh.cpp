#include "../stdafx.h"
#include "Graphics/Mesh.h"
#include "Utils/Utils.h"
#include <sstream>
#include <iostream>
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/normal.hpp"
#include "GL/freeglut.h"
#include <string.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, float boundingRadius) {
	this->vertices = vertices;
	this->originVertices = vertices;
	this->triangleElements = indices;
	this->textures = textures;
	this->boundingRadius = boundingRadius;

	setupMesh();
}

Mesh::~Mesh() {

}

void Mesh::render(GLuint shaderProgram) {
	// Load each texture into the shader samplers.
	GLuint diffuseNum = 0;
	GLuint specularNum = 0;
	GLuint normalNum = 0;
	// Default to no normal map.
	ShaderLoader::setShaderValue(shaderProgram, ShaderLoader::Vars::MAT_HAS_NORMALMAP, (GLuint)0);
	for (GLuint i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		std::string name = textures[i].type;
		int number = 0;
		if (name == ShaderLoader::Vars::MAT_DIFFUSE) number = ++diffuseNum;
		else if (name == ShaderLoader::Vars::MAT_SPECULAR) number = ++specularNum;
		else if (name == ShaderLoader::Vars::MAT_NORMAL) {
			number = ++normalNum;
			ShaderLoader::setShaderValue(shaderProgram, ShaderLoader::Vars::MAT_HAS_NORMALMAP, (GLuint)1);
		}

		// Bind the texture to the sampler location in the shader.
		ShaderLoader::setShaderValue(shaderProgram, (name + std::to_string(number)).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// Material settings.
	ShaderLoader::setShaderValue(shaderProgram, ShaderLoader::Vars::MAT_DIFFUSE_COLOUR, material.diffuse);
	ShaderLoader::setShaderValue(shaderProgram, ShaderLoader::Vars::MAT_SPECULAR_COLOUR, material.specular);
	ShaderLoader::setShaderValue(shaderProgram, ShaderLoader::Vars::MAT_SHININESS, material.shininess);
	
	// Draw.
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, triangleElements.size(), GL_UNSIGNED_INT, 0);
	// Unbind the VAO.
	glBindVertexArray(0);

	// Set textures back to defaults.
	for (GLuint i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Mesh::setupMesh() {
	// Create vertex array and buffers.
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Vertex buffer.
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Copy the vertices array into the vertex buffer.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// Element buffer.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// Copy face indices to the element buffer.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleElements.size() * sizeof(GLuint), &triangleElements[0], GL_STATIC_DRAW);

	// Vertex position attribute (vector 3).
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	// Vertex normal attribute (vector 3).
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

	// Vertex texture coordinate attribute (vector 2).
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));

	// Vertex tangent attribute (vector 3).
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tangent));

	glBindVertexArray(0);
}

