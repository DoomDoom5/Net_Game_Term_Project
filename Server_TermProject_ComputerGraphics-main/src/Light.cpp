#include "stdafx.h"
#include "Light.h"
#include "Object.h"
#include "Shader.h"

Light::Light() : Object()
{
	mObject = new SharedObject(GetIdentityModelObject(Models::GeoSphere));
	mObject->SetColor(ORANGE);
	mObject->SetPivot(&mPosition);
}

GLvoid Light::Draw() const
{
	//object->SetPosition(position);
	shd::SetShader(Shader::Light, "light.pos", mPosition);
	mObject->Draw();
}
GLvoid Light::ToggleLight()
{
	if (isLightOn)
	{
		shd::Use(Shader::Light);
		shd::SetShader(Shader::Light, "light.color", glm::vec3(MyColor(BLACK)));
		shd::Use(Shader::Texture);
		shd::SetShader(Shader::Texture, "light.color", glm::vec3(MyColor(BLACK)));
	}
	else
	{
		shd::Use(Shader::Light);
		shd::SetShader(Shader::Light, "light.color", glm::vec3(MyColor(WHITE)));
		shd::Use(Shader::Texture);
		shd::SetShader(Shader::Texture, "light.color", glm::vec3(MyColor(WHITE)));
	}
	isLightOn = !isLightOn;
}

GLvoid InitLight()
{
	shd::Use(Shader::Light);
	shd::SetShader(Shader::Light, "light.color", glm::vec3(MyColor(WHITE)));
	shd::SetShader(Shader::Light, "light.intensity", 1.0f);
	shd::SetShader(Shader::Light, "light.ambient", 0.3f);
	shd::SetShader(Shader::Light, "light.diffuse", 1.0f);
	shd::SetShader(Shader::Light, "light.specular", 1.0f);
	shd::SetShader(Shader::Light, "light.shininess", 128.0f);

	shd::Use(Shader::Texture);
	shd::SetShader(Shader::Texture, "light.color", glm::vec3(MyColor(WHITE)));
	shd::SetShader(Shader::Texture, "light.intensity", 1.0f);
	shd::SetShader(Shader::Texture, "light.ambient", 0.3f);
	shd::SetShader(Shader::Texture, "light.diffuse", 1.0f);
	shd::SetShader(Shader::Texture, "light.specular", 1.0f);
	shd::SetShader(Shader::Texture, "light.shininess", 128.0f);
}