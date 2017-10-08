#version 400 core

uniform vec3 lightColour;

out vec4 colour;


void main(void)
{
	colour = vec4(lightColour, 1);
}