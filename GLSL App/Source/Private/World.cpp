#include "../stdafx.h"
#include "World.h"
#include "utils\Utils.h"
#include "glm\gtx\string_cast.hpp"
#include "glm\gtx\vector_angle.hpp"
#include "glm\gtx\rotate_vector.hpp"
#include <ctime>
#include "glm\gtc\type_ptr.hpp"
#include "Input/InputManager.h"
#include "Components/InteractableComponent.h"


World::World() : camera(this), lightEntity(this) {
	// Init camera.
	camera.setPosition(0, 0, 5);
}

World::~World() {
	/*for (auto* entity : entities) {
		delete entity;
	}*/
}

void World::init() {
	createShaders();
	inputManager.init();

	lightEntity = Entity(this, "assets/models/ball.obj");

	// Number of lights defined in the object fragment shader.
	addLight(glm::vec3(10, 5, -5), glm::vec3(0.2), glm::vec3(0.8), glm::vec3(1));
	addLight(glm::vec3(-5, -5, 0), glm::vec3(0), glm::vec3(0.2), glm::vec3(0.8));

	// Create the skybox cube mesh.
	std::vector<Vertex> verts {
		// Front
		Vertex(-1, -1, 1),
		Vertex(1, -1, 1),
		Vertex(1, 1, 1),
		Vertex(-1, 1, 1),
		// Back
		Vertex(-1, -1, -1),
		Vertex(1, -1, -1),
		Vertex(1, 1, -1),
		Vertex(-1, 1, -1)
	};
	// Cube indices for the above vertices.
	std::vector<GLuint> indices = {
		// Front
		0, 1, 2,
		2, 3, 0,
		// Top
		1, 5, 6,
		6, 2, 1,
		// Back
		7, 6, 5,
		5, 4, 7,
		// Bottom
		4, 0, 3,
		3, 7, 4,
		// Left
		4, 5, 1,
		1, 0, 4,
		// Right
		3, 2, 6,
		6, 7, 3
	};
	// Texture is not bound since it is used differently to usual and handled during the world rendering.
	skybox.addMesh(verts, indices, std::vector<Texture>(), 0);
}

Entity::EntityPtr World::createEntity(GLchar* path, Model::ImportSettings importSettings/* = Model::ImportSettings()*/) {
	entities.push_back(Entity::EntityPtr(new Entity(this, path, importSettings)));
	entitiesAndLights.push_back(entities.back());
	return entities.back();
}
Entity::EntityPtr World::createEntity(Model model) {
	entities.push_back(Entity::EntityPtr(new Entity(this, model)));
	entitiesAndLights.push_back(entities.back());
	return entities.back();
}

void World::addLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
	lights.push_back(Light::Lightptr(new Light(this, "assets/models/ball.obj")));
	Light::Lightptr light = lights.back();
	// Allow lights to be moved around.
	light->addComponent<InteractableComponent>();
	// Apply settings.
	light->setPosition(position);
	light->ambient = ambient;
	light->diffuse = diffuse;
	light->specular = specular;
	entitiesAndLights.push_back(light);
}

/*
void World::addLight(Light& light) {
	lights.push_back(light);
}*/

void World::setSkyboxTexture(const char* rightfile, const char* leftFile, const char* topFile, const char* bottomFile, const char* backFile, const char* frontFile) {
	skyboxTexture = Utils::loadCubemap(rightfile, leftFile, topFile, bottomFile, backFile, frontFile);
}

void World::update(float deltaTime) {
	camera.update(deltaTime);
	inputManager.update(entitiesAndLights);	

	for (auto& entity : entitiesAndLights) {
		if (entity) entity->update(deltaTime);
	}
}

void World::render() {
	// --- Lights
	glUseProgram(lightShader);
	updateVP(lightShader);
	// Render each light.
	for (unsigned int i=0; i < lights.size(); i++) {
		// Update the object shader lights.
		glUseProgram(objectShader);
		ShaderLoader::setShaderValue(objectShader, ("lights[" + std::to_string(i) + "].position").c_str(), lights[i]->getPosition());
		ShaderLoader::setShaderValue(objectShader, ("lights[" + std::to_string(i) + "].ambient").c_str(), lights[i]->ambient);
		ShaderLoader::setShaderValue(objectShader, ("lights[" + std::to_string(i) + "].diffuse").c_str(), lights[i]->diffuse);
		ShaderLoader::setShaderValue(objectShader, ("lights[" + std::to_string(i) + "].specular").c_str(), lights[i]->specular);

		// Render a sphere for the light using the light shader.
		glUseProgram(lightShader);
		ShaderLoader::setShaderValue(lightShader, "lightColour", lights[i]->diffuse);
		// Render the light.
		lights[i]->render(lightShader);
	}

	// --- Render objects using the object shader.
	glUseProgram(objectShader);

	updateVP(objectShader);
	// Also send camera position to the shader for specular lighting calculations.
	ShaderLoader::setShaderValue(objectShader, ShaderLoader::Vars::VIEW_POSITION, camera.getPosition());

	// Render entities.
	for (auto& entity : entities) {
		if (entity) entity->render(objectShader);
	}

	// --- Skybox
	// Change depth method so values that are equal to the depth buffer content are still shown.
	glDepthFunc(GL_LEQUAL);
	// Render with the skybox shader.
	glUseProgram(skyboxShader);
	// Send view and projection.
	ShaderLoader::setShaderValue(skyboxShader, ShaderLoader::Vars::PROJECTION, camera.projectionMatrix);
	// Remove translation component from the view matrix so the skybox stays stationary.
	ShaderLoader::setShaderValue(skyboxShader, ShaderLoader::Vars::VIEW, glm::mat4(glm::mat3(camera.viewMatrix)));
	// Use the cubemap texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	skybox.render(skyboxShader);
	glDepthFunc(GL_LESS);
}

void World::updateVP(GLuint& shaderProgram) {
	// View.
	ShaderLoader::setShaderValue(shaderProgram, ShaderLoader::Vars::VIEW, camera.viewMatrix);

	// Projection.
	ShaderLoader::setShaderValue(shaderProgram, ShaderLoader::Vars::PROJECTION, camera.projectionMatrix);
}

void World::createShaders() {
	objectShader = ShaderLoader::createShaderProgram("shaders/ObjectShader/ObjectVertex.glsl", "shaders/ObjectShader/ObjectFragment.glsl");
	lightShader = ShaderLoader::createShaderProgram("shaders/LightShader/LightVertex.glsl", "shaders/LightShader/LightFragment.glsl");
	skyboxShader = ShaderLoader::createShaderProgram("shaders/CubemapShader/CubemapVertex.glsl", "shaders//CubemapShader/CubemapFragment.glsl");
}
