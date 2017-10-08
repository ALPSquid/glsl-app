#version 400 core

// Renders an object with no shading.
layout (location = 0) in vec3 position;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
mat4 modelViewProjection;


void main(void) 
{
	modelViewProjection = projection * view * model;
	gl_Position = modelViewProjection * vec4(position, 1.0f);	
}