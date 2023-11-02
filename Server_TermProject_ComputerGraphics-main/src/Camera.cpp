#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
{
	
}

Camera::Camera(const glm::vec3& position)
{
	this->SetPosition(position);
}
