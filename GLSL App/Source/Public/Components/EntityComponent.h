#pragma once
#include "World.h"

class Entity;

/** A component that can be added to an entity and do things! */
class EntityComponent {

private:
	/** Entity that this component is attached to. */
	Entity* owner = nullptr;

	/** Flag for whether beginPlay has been called. */
	bool hasBegunPlay = false;

public:
	EntityComponent(Entity* owner);
	~EntityComponent();

	/** Called the first time this entity is updated. */
	virtual void beginPlay();

	/** Main update function called each frame. */
	virtual void update(float deltaTime);

	/** Returns the entity this component is attached to */
	inline Entity* getOwner() { return owner; };
	
};

