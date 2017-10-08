#pragma once

#include "Transform.h"
#include "Graphics/Model.h"
#include <memory>


class EntityComponent;

// Base entity class for objects that can be manipulated.
class Entity : public ITransform {

public:
	typedef std::shared_ptr<Entity> EntityPtr;
	typedef std::shared_ptr<EntityComponent> ComponentPtr;

	Model model;

protected:
	//std::vector<EntityComponent*> components;
	std::vector<ComponentPtr> components;

private:
	// World instance this entity is in.
	class World* world;

public:
	Entity(World* world);
	/** Parameter: GLchar* path  Path to the model file to use. */
	Entity(World* world, GLchar* path, Model::ImportSettings importSettings = Model::ImportSettings());
	Entity(World* world, Model model);

	~Entity();	

	virtual void update(float deltaTime);
	virtual void render(GLuint shaderProgram);

	/** Adds a component to be owned by this entity. */
	template<typename T>
	inline T* addComponent() {
		components.emplace_back(new T(this));
		return dynamic_cast<T*>(components.back().get());
	}


	/**
	* Returns the component of the specified type attached to this entity, or null if one doesn't exist.
	*/
	template<typename T>
	inline T* getComponent() {
		for (auto component : components) {
			// If the cast is not null, the component is of type T.
			T* castComponent = dynamic_cast<T*>(component.get());
			if (castComponent) return castComponent;
		}
		// Component not found, return null.
		return nullptr;
	}

	inline World* getWorld() { return world; };
};

