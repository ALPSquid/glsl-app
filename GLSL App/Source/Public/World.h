#pragma once
#include <vector>
#include "glm\gtc\matrix_transform.hpp"
#include "Graphics/Model.h"
#include "Utils\Utils.h"
#include "Entities/Entity.h"
#include "Entities/Camera.h"
#include "Input/InputManager.h"
#include "Entities/Light.h"


class World {

protected:
	// Input manager for the world.
	class InputManager inputManager;

	// Camera to use when rendering the world.
	Camera camera;

	// Skybox cube model.
	Model skybox;
	// Cubemap texture to use for the skybox.
	GLuint skyboxTexture;

	// Lights in the world.
	std::vector<Light::Lightptr> lights;
	// Entities in the world.
	std::vector<Entity::EntityPtr> entities;
	// Combined array of Entities and lights in the world.
	std::vector<Entity::EntityPtr> entitiesAndLights;
	// Entity to render lights with. Reused for each light in the scene.
	Entity lightEntity;

	// Shaders.
	GLuint objectShader;
	GLuint lightShader;
	GLuint skyboxShader;

public:
	World();
	~World();

	/** Call to initialize the world. Must be done after the OpenGL context is created. */
	void init();

	/**
	* Creates an entity in the world with the specified model and returns a reference.
	* Parameter: GLchar* path  Path to the model to use for the entity.
	* Returns: EntityPtr  Created entity reference.
	*/
	Entity::EntityPtr createEntity(GLchar* path, Model::ImportSettings importSettings = Model::ImportSettings());
	Entity::EntityPtr createEntity(Model model);

	// Adds a light to the world.
	//void addLight(Light& light);
	void addLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

	/** Create and set a new skybox texture. Each file is a face on the cube. */
	void setSkyboxTexture(const char* rightfile, const char* leftFile, const char* topFile, const char* bottomFile, const char* backFile, const char* frontFile);

	// Update entities in the world.
	void update(float deltaTime);
	// Render the world.
	void render();

	/** Updates the View, Projection uniforms for a shader. */
	void updateVP(GLuint& ShaderProgram);
	
	inline InputManager& getInputManager() { return inputManager; };
	inline Camera& getCamera() { return camera; };
	//inline std::vector<Entity*>& getEntities() { return entities; };
	inline std::vector<Entity::EntityPtr>& getEntities() { return entities; };

protected:
	void createShaders();
};
