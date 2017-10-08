#include "../stdafx.h"
#include "Entities/Camera.h"
#include "glm\gtc\matrix_transform.hpp"
#include "World.h"
#include "Input/InputManager.h"


Camera::Camera(World* world) : Entity(world) {

	// --- Input bindings.

	// Forwards
	world->getInputManager().addTriggerBinding('w', InputManager::INPUT_PRESSED, [this]() {
		cameraForwardVal = 1;
	});
	// Backwards
	world->getInputManager().addTriggerBinding('s', InputManager::INPUT_PRESSED, [this]() {
		cameraForwardVal = -1;
	});
	// Stop forward/backward.
	world->getInputManager().addTriggerBinding({ 'w', 's' }, InputManager::INPUT_RELEASED, [this]() {
		cameraForwardVal = 0;
	});

	// Left
	world->getInputManager().addTriggerBinding('a', InputManager::INPUT_PRESSED, [this]() {
		cameraRightVal = -1;
	});
	// Right
	world->getInputManager().addTriggerBinding('d', InputManager::INPUT_PRESSED, [this]() {
		cameraRightVal = 1;
	});
	// Stop right/left
	world->getInputManager().addTriggerBinding({'a', 'd'}, InputManager::INPUT_RELEASED, [this]() {
		cameraRightVal = 0;
	});

	// Up
	world->getInputManager().addTriggerBinding({' ', 'e'}, InputManager::INPUT_PRESSED, [this]() {
		cameraUpVal = 1;
	});
	// Down
	world->getInputManager().addTriggerBinding({'x', 'q'}, InputManager::INPUT_PRESSED, [this]() {
		cameraUpVal = -1;
	});
	// Stop Up/Down
	world->getInputManager().addTriggerBinding({' ', 'x', 'q', 'e'}, InputManager::INPUT_RELEASED, [this]() {
		cameraUpVal = 0;
	});

	// Rotation flag.
	world->getInputManager().addTriggerBinding(InputManager::MOUSE_RIGHT, InputManager::INPUT_PRESSED, [this]() {
		rotateCamera = true;
		// Hide the cursor when looking around.
		glutSetCursor(GLUT_CURSOR_NONE);
	});
	world->getInputManager().addTriggerBinding(InputManager::MOUSE_RIGHT, InputManager::INPUT_RELEASED, [this]() {
		rotateCamera = false;
		// Show the cursor when looking around stops.
		glutSetCursor(GLUT_CURSOR_INHERIT);
		// Snap cursor back to centre of the screen when looking stops.
		glutWarpPointer(screenWidth / 2, screenHeight / 2);
	});

	// Look
	world->getInputManager().addValueBinding(InputManager::EMouseAxis::MOUSE_X, std::bind(&Camera::lookRight, this, std::placeholders::_1));
	world->getInputManager().addValueBinding(InputManager::EMouseAxis::MOUSE_Y, std::bind(&Camera::lookUp, this, std::placeholders::_1));
}

void Camera::update(float deltaTime) {
	Entity::update(deltaTime);

	// Update camera position along the direction its facing by the camera speed scaled by the input.
	// The movement is scaled by delta time to make it framerate independent (move the same amount each second, regardless of framerate)
	bool cameraMoved = false;
	// Forward
	if (cameraForwardVal != 0) {
		move(((flySpeed * cameraForwardVal) * deltaTime) * getForwardVector());
		cameraMoved = true;
	}
	// Right
	if (cameraRightVal != 0) {
		move(((flySpeed * cameraRightVal) * deltaTime) * getRightVector());
		cameraMoved = true;
	}
	// Up
	if (cameraUpVal != 0) {
		move(((flySpeed * cameraUpVal) * deltaTime) * getUpVector());
		cameraMoved = true;
	}

	// Update the camera matrices if it moved.
	if (cameraMoved) updateMatrices();
}

void Camera::updateMatrices(float FOV, float screenWidth, float screenHeight, float nearClippingPlane, float farClippingPlane) {
	this->FOV = FOV;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->nearClippingPlane = nearClippingPlane;
	this->farClippingPlane = farClippingPlane;

	updateMatrices();
}

void Camera::updateMatrices() {
	projectionMatrix = glm::perspective(FOV, screenWidth / screenHeight, nearClippingPlane, farClippingPlane);
	viewMatrix = glm::lookAt(getPosition(), getPosition() + getForwardVector(), getUpVector());
}

void Camera::lookRight(float deltaDegrees) {
	if (!rotateCamera) return;

	rotateBy(deltaDegrees, UP_VECTOR); // Camera should never roll, so rotate around the world up vector.
	updateMatrices();
}

void Camera::lookUp(float deltaDegrees) {
	if (!rotateCamera) return;

	// Clamp pitch.
	float deltaPitch = maxPitch - abs(pitch + deltaDegrees);
	// If the pitch delta won't go over maxPitch or the target rotation is in the opposite direction to the current pitch.
	if (deltaPitch > 0 || ((pitch < 0 && deltaDegrees > 0) || (pitch > 0 && deltaDegrees < 0))) {
		// Use the smaller of deltaPitch and desired rotation delta.
		float rotAmount = (deltaPitch < abs(deltaDegrees)) ? deltaPitch : abs(deltaDegrees);
		// Keep sign.
		rotAmount = (deltaDegrees < 0) ? -rotAmount : rotAmount;
		// Rotate by the final pitch delta.
		rotateBy(rotAmount, getRightVector());
		pitch += rotAmount;
	}
	updateMatrices();
}
