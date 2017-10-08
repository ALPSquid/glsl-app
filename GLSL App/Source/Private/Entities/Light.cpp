#include "../stdafx.h"
#include "Entities/Light.h"


Light::Light(World* world) : Entity(world) {}

Light::Light(World* world, GLchar* path) : Entity(world, path) {}

