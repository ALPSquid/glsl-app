#include "../stdafx.h"
#include "Entities/Entity.h"
#include "Components/EntityComponent.h"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\quaternion.hpp"



Entity::Entity(World* world) {
	this->world = world;
}

Entity::Entity(World* world, GLchar* path, Model::ImportSettings importSettings/* = Model::ImportSettings()*/) {
	this->world = world;
	model = Model(path, importSettings);
}


Entity::Entity(World* world, Model model) {
	this->world = world;
	this->model = Model(model);
}

Entity::~Entity() {
	// Clean up components.
	/*for (auto* component : components) {
		delete component;
	}*/
}

void Entity::update(float deltaTime) {
	// Update components.
	for (auto& component : components) {
		if (component) component->update(deltaTime);
	}
}

void Entity::render(GLuint shaderProgram) {
	// Update model matrices.
	glm::mat4 modelMatrix = glm::mat4(1.f);
	modelMatrix = glm::translate(modelMatrix, getPosition());
	// Apply rotation.
	modelMatrix *= glm::toMat4(getRotation());
	// Scale.
	modelMatrix = glm::scale(modelMatrix, getScale());

	// Send model matrix to shader.
	ShaderLoader::setShaderValue(shaderProgram, ShaderLoader::Vars::MODEL, modelMatrix);

	// Render the model.
	model.render(shaderProgram);
}
