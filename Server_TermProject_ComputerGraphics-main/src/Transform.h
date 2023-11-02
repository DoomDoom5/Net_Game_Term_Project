#pragma once
#include "stdafx.h"

class Camera;

namespace xform {
	/* WorldTransform을 반환, 사용 X */
	glm::mat4 GetWorld();

	/* Camera class에서 View Tansform 추출*/
	glm::mat4 GetView(const Camera* camera);
	/* Camera class에서 Projection Tansform 추출*/
	glm::mat4 GetProj(const Camera* camera);

	/* shader에 있는 name에 mat4(1.0f) 적용*/
	GLvoid Disable(const Shader& shader, const GLchar* name);
}