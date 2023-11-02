#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D outTexture;

void main(void)
{
	vec4 TexColor = texture(outTexture, TexCoord);
	if (TexColor.a < 0.1)
		discard;

	FragColor = TexColor;
}