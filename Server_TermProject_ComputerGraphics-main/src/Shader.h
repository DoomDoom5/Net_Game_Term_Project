#pragma once
#include "stdafx.h"

namespace shd {
	/* 등록된 모든 Shader 생성 */
	GLvoid Init();

	GLint GetShaderProgram(const Shader& shader);
	GLvoid Use(const Shader& shader);

	/* shader에 있는 name에 mat4 적용 */
	GLvoid SetShader(const Shader& shader, const GLchar* name, const glm::mat4& transform);
	/* shader에 있는 name에 vec3 적용 */
	GLvoid SetShader(const Shader& shader, const GLchar* name, const glm::vec3& value);
	/* shader에 있는 name에 GLfloat 적용 */
	GLvoid SetShader(const Shader& shader, const GLchar* name, const GLfloat& value);
}