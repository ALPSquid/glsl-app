#pragma once

#include "EntityComponent.h"
#include "glm/glm.hpp"


/* Rotates the entity indefinitely unless its selected when the user can rotate it. */
class RotatingComponent : public EntityComponent {

public:
	/** Speed to rotate in degrees per second. */
	float speed;
	/** Axis to rotate around. */
	glm::vec3 axis;
	/** Whether the axis should be in local or global space */
	bool localSpace = true;

protected:
	// Pointer to the interactable. component of the owning entity, if there is one.
	class InteractableComponent* interactableComponent;

public:
	RotatingComponent(Entity* owner);

	void beginPlay() override;

	void update(float deltaTime) override;
};

