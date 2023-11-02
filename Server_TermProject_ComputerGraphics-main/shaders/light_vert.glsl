#version 330 core

layout (location=0) in vec3 vPos;
layout (location=1) in vec3 vNormal;

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projTransform;

out VS_OUT{
	vec3 FragPos;
	vec3 Normal;
}vs_out;

void main(void)
{
	gl_Position = projTransform * viewTransform * modelTransform * vec4(vPos, 1.0);
	
	vs_out.FragPos = vec3(modelTransform * vec4(vPos, 1.0));

	vs_out.Normal = vec3(modelTransform * vec4(vNormal, 0.0));
}