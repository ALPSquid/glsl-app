#version 400 core

struct Material {
	// Diffuse maps.
	sampler2D diffuse1;
	sampler2D diffuse2;

	// Specular maps.
	sampler2D specular1;
	sampler2D specular2;

	// Normal maps.
	sampler2D normal1;
	sampler2D normal2;

	// General settings.
	vec3 diffuseColour;
	vec3 specularColour;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 ambient; // Ambient intensity & colour.
	vec3 diffuse; // Diffuse intensity & colour.
	vec3 specular; // Specular intensity & colour.
};


uniform Material material;
#define NUM_LIGHTS 2
uniform Light lights[NUM_LIGHTS];

uniform vec3 viewPosition;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPosition;
in mat3 tbn;

out vec4 colour;

vec4 calcLight(Light light, vec4 objDiffuse, vec4 objSpecular) {
	// Ambient lighting.
	vec3 ambientLighting = light.ambient;

	// Diffuse lighting.
	vec3 lightDir = normalize(light.position - fragPosition);
	vec3 diffuseLighting = max(dot(normal, lightDir), 0.f) * light.diffuse;

	// Blinn-Phong Specular value based on angle between the normal and the half vector between the view and the light.
	vec3 viewDir = normalize(viewPosition - fragPosition);
	vec3 halfDir = normalize(lightDir + viewDir);
	vec3 specularLighting = pow(max(dot(normal, halfDir), 0.f), material.shininess) * light.specular;

	// Combine lighting components.
	vec4 fragColour = vec4(ambientLighting, 1) * objDiffuse;
	fragColour += vec4(diffuseLighting, 1) * objDiffuse;
	fragColour += vec4(specularLighting, 1) * objSpecular;

	return fragColour;
}

void main(void)
{
	// Lights
	/*light1.position = vec3(6, 5, 5);
	light1.ambient = vec3(0.2);
	light1.diffuse = vec3(1);
	light1.specular = vec3(1);*/

	// Diffuse map colour of the fragment.
	vec4 diffuse1 = texture(material.diffuse1, texCoord);
	vec4 diffuse2 = texture(material.diffuse2, texCoord);
	vec4 objDiffuse = (diffuse1 + diffuse2) * vec4(material.diffuseColour, 1);

	// Specular map colour of the fragment.
	vec4 specular1 = texture(material.specular1, texCoord);
	vec4 specular2 = texture(material.specular2, texCoord);
	vec4 objSpecular = (specular1 + specular2) * vec4(material.specularColour, 1);
	
	for (int i=0; i < NUM_LIGHTS; i++) {
		colour += calcLight(lights[i], objDiffuse, objSpecular);
	}
}