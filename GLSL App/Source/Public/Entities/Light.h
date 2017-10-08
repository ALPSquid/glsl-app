#pragma once
#include "Entity.h"

/** Entity used for lighting. */
class Light : public Entity {

public:
	typedef std::shared_ptr<Light> Lightptr;

	glm::vec3 ambient; // ambient intensity
	glm::vec3 diffuse; // diffuse intensity
	glm::vec3 specular; // specular intensity

public:
	Light(World* world);
	Light(World* world, GLchar* path);
};

