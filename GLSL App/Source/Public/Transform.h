#pragma once

#include "Utils/Utils.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

/** A transform containing position, rotation and directional data. */
class ITransform {

private:
	glm::vec3 position;
	glm::fquat rotation;
	glm::vec3 scale = glm::vec3(1);

	// Directional vectors. Defaulted to global direction vectors.
	glm::vec3 forwardVector = FORWARD_VECTOR;
	glm::vec3 rightVector = RIGHT_VECTOR;
	glm::vec3 upVector = UP_VECTOR;

public:
	/**
	* Sets the position of the entity to the specified position.
	* Parameter: glm::vec3 newPosition  Position to move the entity to.
	*/
	void setPosition(glm::vec3 newPosition);
	void setPosition(float x, float y, float z);

	/**
	* Translates the entity by the specified amount.
	* Parameter: glm::vec3 offset  Vector to offset by.
	*/
	void move(glm::vec3 offset);
	void move(float x, float y, float z);

	/**
	* Sets the rotation of the entity.
	* Parameter: glm::quat newRotation  New rotation.
	*/
	virtual void setRotation(glm::quat newRotation);
	void setRotation(glm::vec3 newRotation);

	/**
	* Rotates the entity by the x degrees around y axis.
	* Parameter: float degrees  Amount to rotate by.
	* Parameter: glm::vec3 axis  Axis to rotate around (will be normalised).
	* Parameter: bool global  Whether the axis should be applied in local or global space. Default = local;
	*/
	void rotateBy(float degrees, glm::vec3 axis, bool global=false);


	/**
	* Sets the scale of the entity.
	* Parameter: glm::vec3 scaleFactor  Scale factor for each axis.
	*/
	void setScale(glm::vec3 scaleFactor);
	void setScale(float scaleFactor);

	inline glm::vec3 getPosition() { return position; }
	inline glm::fquat getRotation() { return rotation; }
	inline glm::vec3 getScale() { return scale; };

	inline glm::vec3 getForwardVector() { return forwardVector; }
	inline glm::vec3 getRightVector() { return rightVector; }
	inline glm::vec3 getUpVector() { return upVector; }
};