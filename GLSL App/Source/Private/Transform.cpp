#include "../stdafx.h"
#include "Transform.h"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/string_cast.hpp"


void ITransform::setPosition(glm::vec3 newPosition) {
	position = newPosition;
}

void ITransform::setPosition(float x, float y, float z) {
	setPosition(glm::vec3(x, y, z));
}

void ITransform::move(glm::vec3 offset) {
	position += offset;
}

void ITransform::move(float x, float y, float z) {
	move(glm::vec3(x, y, z));
}

void ITransform::setRotation(glm::quat newRotation) {
	rotation = newRotation;
	// Rotate identity direction vectors to match.
	forwardVector = glm::normalize(FORWARD_VECTOR * rotation);
	rightVector = glm::normalize(RIGHT_VECTOR * rotation);
	upVector = glm::normalize(UP_VECTOR * rotation);
}
void ITransform::setRotation(glm::vec3 newRotation) {
	setRotation(glm::quat(newRotation));
}

void ITransform::rotateBy(float degrees, glm::vec3 axis, bool global/*=false*/) {
	float radians = glm::radians(degrees);
	axis = glm::normalize(axis);
	// Rotation space depends on which way round the quaternions are applied.
	// Global (Existing rot applied to delta rot)
	if (global) setRotation(glm::angleAxis(radians, axis) * rotation);
	// Local (Delta rot applied to existing rot).
	else setRotation(rotation * glm::angleAxis(radians, axis));
}


void ITransform::setScale(glm::vec3 scaleFactor) {
	scale = scaleFactor;
}
void ITransform::setScale(float scaleFactor) {
	setScale(glm::vec3(scaleFactor));
}
