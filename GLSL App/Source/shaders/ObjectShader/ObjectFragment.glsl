#version 400 core

struct Material {
	// Diffuse maps.
	sampler2D diffuse1;
	sampler2D diffuse2;

	// Specular maps.
	sampler2D specular1;
	sampler2D specular2;

	// Normal map.
	sampler2D normal1;
	int hasNormalMap;

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
in mat3 tbn; // Tangent, bitangent normal matrix for converting from tangent to world space.
vec3 worldNormal; // Tangent normal converted with the tangent, bitangent, normal matrix to world space.

out vec4 colour;

vec4 calcLight(Light light, vec4 objDiffuse, vec4 objSpecular) {
	// Ambient lighting.
	vec3 ambientLighting = light.ambient;

	// Diffuse lighting.
	vec3 lightDir = normalize(light.position - fragPosition);
	vec3 diffuseLighting = max(dot(worldNormal, lightDir), 0.f) * light.diffuse;

	// Blinn-Phong Specular value based on angle between the normal and the half vector between the view and the light.
	vec3 viewDir = normalize(viewPosition - fragPosition);
	vec3 halfDir = normalize(lightDir + viewDir);
	vec3 specularLighting = pow(max(dot(worldNormal, halfDir), 0.f), material.shininess) * light.specular;

	// Combine lighting components.
	vec4 fragColour = vec4(ambientLighting, 1) * objDiffuse;
	fragColour += vec4(diffuseLighting, 1) * objDiffuse;
	fragColour += vec4(specularLighting, 1) * objSpecular;

	return fragColour;
}

void main(void)
{
	// Diffuse map colour of the fragment.
	vec4 diffuse1 = texture(material.diffuse1, texCoord);
	// For second textures.
	//vec4 diffuse2 = texture(material.diffuse2, texCoord);
	//vec4 objDiffuse = (diffuse1 + diffuse2) * vec4(material.diffuseColour, 1);
	vec4 objDiffuse = (diffuse1) * vec4(material.diffuseColour, 1);

	// Specular map colour of the fragment.
	vec4 specular1 = texture(material.specular1, texCoord);
	// For second textures.
	//vec4 specular2 = texture(material.specular2, texCoord);
	//vec4 objSpecular = (specular1 + specular2) * vec4(material.specularColour, 1);
	vec4 objSpecular = (specular1) * vec4(material.specularColour, 1);

	worldNormal = normal; // Default to the normal.	
	// If there's a normal map, use the normal map sample normal and convert to
	// world-space using the tangent to world space matrix. Normal map components are R = x, G = Y, B = z.
	if (material.hasNormalMap == 1) {
		// Get normal map sample.
		worldNormal = texture(material.normal1, texCoord).rgb;
		// Convert to be in -1, 1 range.
		worldNormal = normalize(worldNormal * 2.0 - 1.0);		
		// Convert to world-space using the matrix created in the vertex shader.
		worldNormal = normalize(tbn * worldNormal);
	}
	
	// Calculate Blinn-phong shading for each light.
	for (int i=0; i < NUM_LIGHTS; i++) {
		colour += calcLight(lights[i], objDiffuse, objSpecular);
	}
	//colour = vec4(1);
}