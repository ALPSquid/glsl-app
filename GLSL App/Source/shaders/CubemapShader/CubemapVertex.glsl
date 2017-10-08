#version 400 core

// Renders a cubemapped object.
layout (location = 0) in vec3 position;

uniform mat4 view;
uniform mat4 projection;

out vec3 texCoord;


void main(void) 
{
	texCoord = position;
	gl_Position = (projection * view * vec4(position, 1.0f)).xyww;	
}