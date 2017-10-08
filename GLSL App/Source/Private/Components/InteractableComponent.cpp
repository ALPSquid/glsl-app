#include "../stdafx.h"
#include "Components/InteractableComponent.h"


InteractableComponent::InteractableComponent(Entity* owner) : EntityComponent(owner) {
	// Bind rotate button.
	owner->getWorld()->getInputManager().addTriggerBinding(InputManager::MOUSE_LEFT, InputManager::INPUT_PRESSED, [this]() {
		selectionPressed = true;
		selected = isHovered;
	});
	owner->getWorld()->getInputManager().addTriggerBinding(InputManager::MOUSE_LEFT, InputManager::INPUT_RELEASED, [this]() {
		selectionPressed = false;
		selected = false;
	});

	// Bind move button.
	owner->getWorld()->getInputManager().addTriggerBinding('g', InputManager::INPUT_PRESSED, [this]() {
		movePressed = true;
	});
	owner->getWorld()->getInputManager().addTriggerBinding('g', InputManager::INPUT_RELEASED, [this]() {
		movePressed = false;
	});

	// Bind scale button.
	owner->getWorld()->getInputManager().addTriggerBinding('f', InputManager::INPUT_PRESSED, [this]() {
		scalePressed = true;
	});
	owner->getWorld()->getInputManager().addTriggerBinding('f', InputManager::INPUT_RELEASED, [this]() {
		scalePressed = false;
	});

	// Bind mouse over/out.
	owner->getWorld()->getInputManager().adddEventBinding(owner, InputManager::MOUSE_OVER, [this]() {
		isHovered = true;
	});
	owner->getWorld()->getInputManager().adddEventBinding(owner, InputManager::MOUSE_OUT, [this]() {
		isHovered = false;
	});

	// Mouse movement for rotation.
	owner->getWorld()->getInputManager().addValueBinding(InputManager::EMouseAxis::MOUSE_X, std::bind(&InteractableComponent::manipulateRight, this, std::placeholders::_1));
	owner->getWorld()->getInputManager().addValueBinding(InputManager::EMouseAxis::MOUSE_Y, std::bind(&InteractableComponent::manipulateUp, this, std::placeholders::_1));
}

void InteractableComponent::manipulateUp(float val) {
	if (!selected) return;

	// Movement.
	if (movePressed) getOwner()->move(getOwner()->getWorld()->getCamera().getUpVector() * -val * 0.1f);
	// Uniform scaling.
	else if (scalePressed) getOwner()->setScale(getOwner()->getScale() + -val  * 0.1f);
	// Standard rotation.
	else getOwner()->rotateBy(val, getOwner()->getWorld()->getCamera().getRightVector(), true);
}

void InteractableComponent::manipulateRight(float val) {
	if (!selected) return;

	// Movement.
	if (movePressed) getOwner()->move(getOwner()->getWorld()->getCamera().getRightVector() * val * 0.1f);
	// Uniform scaling.
	else if (scalePressed) getOwner()->setScale(getOwner()->getScale() + val * 0.1f);
	// Standard rotation.
	else getOwner()->rotateBy(val, getOwner()->getWorld()->getCamera().getUpVector(), true);
}