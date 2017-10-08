#include "../stdafx.h"
#include "Components/RotatingComponent.h"
#include "Entities/Entity.h"
#include "Components/InteractableComponent.h"


RotatingComponent::RotatingComponent(Entity* owner) : EntityComponent(owner) {}

void RotatingComponent::beginPlay() {
	EntityComponent::beginPlay();

	// Get a pointer to the interactable component. Will be null if there isn't one.
	interactableComponent = getOwner()->getComponent<InteractableComponent>();
}

void RotatingComponent::update(float deltaTime) {
	EntityComponent::update(deltaTime);

	if (!getOwner()) return;
	
	// Only rotate when not selected, or if there isn't an interactable component.
	if ((interactableComponent && !interactableComponent->isSelected()) || !interactableComponent) {
		getOwner()->rotateBy(speed * deltaTime, axis, localSpace);
	}
}
