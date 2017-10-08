#version 400 core

//uniform float uStretch;
//attribute float someAttribute // A value specific to this vertex. Uniform is global.
//out // Value is passed to the fragment shader.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 vertTexCoord;
layout (location = 3) in vec3 vertTangent;
vec3 bitangent; // Cross of the normal and the tangent.

out vec2 texCoord;
out vec3 normal;
out vec3 fragPosition; // Fragment position in world space.
out mat3 tbn; // Tangent, bitangent, normal matrix.

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
mat4 modelViewProjection;


void main(void) 
{
	texCoord = vertTexCoord;
	// Nnormal in world space.
	normal = normalize(vec3(model * vec4(vertNormal, 0))); 
	// Tangent in world space.
	vertTangent = normalize(vec3(model * vec4(verTangent, 0)));
	// Calculate bitangent as the cross of the normal and the tangent.
	bitangent = cross(normal, verTangent);
	// Construct the tangent, bitangent and normal matrix for normal mapping.
	tbn = mat3(tangent, bitangent, normal);
	
	// Fragment position in world space.
	fragPosition = vec3(model * vec4(position, 1.0f));

	modelViewProjection = projection * view * model;
	gl_Position = modelViewProjection * vec4(position, 1.0f);	
}