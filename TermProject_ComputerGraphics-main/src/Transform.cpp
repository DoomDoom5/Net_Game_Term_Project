#include "stdafx.h"
#include "Transform.h"
#include "Shader.h"
#include "Camera.h"


glm::mat4 xform::GetWorld()
{
	return glm::mat4(1.0f);
}

glm::mat4 xform::GetView(const Camera* camera)
{
	const glm::vec3 eye = camera->GetPviotedPosition();
	const glm::vec3 look = camera->GetLook();
	const glm::vec3 up = camera->GetUp();
	const glm::vec3 center = eye + look;

	return glm::lookAt(eye, center, up);;
}

extern glm::vec3 worldPosition;
glm::mat4 xform::GetProj(const Camera* camera)
{
	constexpr GLfloat farLength = 2000;

	const glm::vec3 cameraPos = camera->GetPviotedPosition();
	const GLfloat size = glm::length(worldPosition - cameraPos) / 2;

	if (camera->IsPersp())
	{
		return glm::perspective(glm::radians(camera->GetFovY()), 1.0f, 0.1f, farLength);
	}
	else
	{
		return glm::ortho(-size, size, -size, size, -farLength, farLength);
	}
}




GLvoid xform::Disable(const Shader& shader, const GLchar* name)
{
	const glm::mat4 transform = glm::mat4(1.0f);
	const GLint modelLocation = glGetUniformLocation(shd::GetShaderProgram(shader), name);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
}