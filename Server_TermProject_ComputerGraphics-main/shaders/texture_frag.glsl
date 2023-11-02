#version 330 core

in VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
}fs_in;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 viewPos;
uniform mat4 lightTransform;

uniform sampler2D outTexture;

struct Light {
	vec3 pos;
	vec3 color;

	// Intensity
	float intensity;	// global intensity
	float ambient;
	float diffuse;
	float specular;
	float shininess;
};

uniform Light light;

void main(void)
{
	vec3 ambient = light.ambient * light.color;

	vec3 normalVector = normalize(fs_in.Normal);
	vec3 lightDir = normalize(light.pos - fs_in.FragPos);
	float diffuseLight = max(dot(normalVector, lightDir), 0.0);
	vec3 diffuse = diffuseLight * light.color * light.diffuse;

	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 reflectDir = reflect(-lightDir, normalVector);
	float specularLight = max(dot(viewDir, reflectDir), 0.0);
	specularLight = pow(specularLight, light.shininess);
	vec3 specular = specularLight * light.color * light.specular;

	vec3 result = (ambient + diffuse + specular) * objectColor * light.intensity;

	vec4 TexColor = texture(outTexture, fs_in.TexCoord) * vec4(result, 1.0);
	if (TexColor.a < 0.1)
		discard;

	FragColor = TexColor;
}