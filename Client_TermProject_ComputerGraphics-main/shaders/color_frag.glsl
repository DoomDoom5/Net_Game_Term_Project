#version 330 core

uniform vec3 objectColor;
out vec4 FragColor;

void main(void)
{
	FragColor = vec4(objectColor, 1.0);
}