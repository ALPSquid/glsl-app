#version 400 core

// Unique code to use as the colour component.
// For more than 255 objects, this would be changed to use a vec4.
uniform int colourCode;

out vec4 colour;


void main(void)
{
	//colour = vec4(colourCode/255.0, 0, 0, 0);
	colour = vec4(colourCode/255.0, 0, 0, 1);
}