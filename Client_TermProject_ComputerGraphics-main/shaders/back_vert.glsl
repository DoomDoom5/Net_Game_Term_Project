#version 330 core

layout (location=0) in vec3 vPos;
layout (location=2) in vec2 vTexCoord;

uniform mat4 modelTransform;

out vec2 TexCoord;

void main(void)
{
	gl_Position = modelTransform * vec4(vPos, 1.0);
	
	TexCoord = vTexCoord;
}