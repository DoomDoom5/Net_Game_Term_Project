#pragma once
#include "stdafx.h"
#include "Object.h"

#define DEFAULT_CAMERA_SPEED 10.0f

class Camera : public Object{
private:
	GLfloat mFovY = 45.0f;
	GLboolean mIsPersp = GL_TRUE;
	GLfloat mSpeed = DEFAULT_CAMERA_SPEED;
public:

	Camera();
	Camera(const glm::vec3& position);

	inline constexpr GLvoid SetFovY(const GLfloat& fovY)
	{
		mFovY = fovY;
	}
	inline constexpr const GLfloat& GetFovY() const
	{
		return mFovY;
	}
	inline constexpr GLvoid SetPerpective(const GLboolean& isPersp)
	{
		mIsPersp = isPersp;
	}
	inline constexpr GLvoid TogglePerpective()
	{
		mIsPersp = !mIsPersp;
	}
	inline constexpr const GLboolean& IsPersp() const
	{
		return mIsPersp;
	}
};