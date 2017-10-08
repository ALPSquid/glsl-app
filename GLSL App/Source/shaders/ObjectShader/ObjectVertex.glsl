#version 400 core

//uniform float uStretch;
//attribute float someAttribute // A value specific to this vertex. Uniform is global.
//out // Value is passed to the fragment shader.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 vertTexCoord;
layout (location = 3) in vec3 vertTangent;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPosition; // Fragment position in world space.
// Tangent, bitangent, normal matrix.
// For performance, it would be better to convert all fragment variables to tangent space in the vertex shader
// as the calculations would be done less times. However for this project, due to time constraints, 
// the tangent normal is instead converted to world space.
out mat3 tbn; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
mat4 modelViewProjection;


void main(void) 
{
	texCoord = vertTexCoord;
	// Ensure the normal is normalised.
	normal = normalize(vertNormal);

	//--- Tangent to world space matrix calculation.
	// Normal in world space.
	vec3 worldNormal = normalize(vec3(model * vec4(vertNormal, 0))); 
	// Tangent in world space.
	vec3 tangent = normalize(vec3(model * vec4(vertTangent, 0)));
	// Calculate bitangent as the cross of the normal and the tangent.
	vec3 bitangent = cross(tangent, normal);
	// Construct the tangent to world-space matrix for normal mapping.
	tbn = mat3(tangent, bitangent, worldNormal);
	//--- 

	// Fragment position in world space.
	fragPosition = vec3(model * vec4(position, 1.0f));

	modelViewProjection = projection * view * model;
	gl_Position = modelViewProjection * vec4(position, 1.0f);	
}