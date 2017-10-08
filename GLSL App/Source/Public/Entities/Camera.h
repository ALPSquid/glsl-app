#pragma once
#include "Entity.h"


class Camera : public Entity {

public:
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;

	float FOV;
	float screenWidth;
	float screenHeight;
	float nearClippingPlane;
	float farClippingPlane;

	float maxPitch = 80;
	// Amount of units to move per second.
	float flySpeed = 3;

protected:
	// Current camera pitch. Used to clamp between maxPitch.
	float pitch = 0;

	/** Forward movement scalar between -1 and 1.  */
	float cameraForwardVal;
	/** Right movement scalar between -1 and 1.  */
	float cameraRightVal;
	/** Up movement scalar between -1 and 1.  */
	float cameraUpVal;

	/** Whether to rotate the camera. */
	bool rotateCamera = false;

public:
	Camera(World* world);

	void update(float deltaTime) override;

	/**
	* Update the camera projection and view matrices.
	* Parameter: float FOV  Field of View.
	* Parameter: float screenWidth  Width of the screen.
	* Parameter: float screenHeight  Height of the screen.
	* Parameter: float nearClippingPlane  near clipping plane distance.
	* Parameter: float farClippingPlane  far clipping plane distance.
	*/
	void updateMatrices(float FOV, float screenWidth, float screenHeight, float nearClippingPlane, float farClippingPlane);
	/** Update the view and projection matrices using the current FOV, aspect ratio and clipping planes. */
	void updateMatrices();

	/** Rotate the camera right by delta. */
	void lookRight(float deltaDegrees);
	/** Rotate the camera up by delta, clamping at maxPitch. */
	void lookUp(float deltaDegrees);

};

