#version 400 core

uniform samplerCube cubemap;

in vec3 texCoord;

out vec4 colour;


void main(void)
{
	// Sample texture coordinate is the position itself.
	colour = texture(cubemap, texCoord);
}