#pragma once
#include "Object.h"

class Light : public Object{
private:
	SharedObject* mObject = nullptr;
	GLboolean isLightOn = GL_TRUE;
public:
	Light();
	GLvoid Draw() const;
	GLvoid ToggleLight();
};

GLvoid InitLight();