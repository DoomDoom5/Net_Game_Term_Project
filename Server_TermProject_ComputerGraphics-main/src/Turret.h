#pragma once
#include "stdafx.h"

class SharedObject;

class TurretManager {
private:
	class Turret {
	private:
		GLfloat mFireDelay = 1.0f;
		GLfloat mCrntFireDelay = 0.0f;
		GLfloat mRadius = 400.0f;
		GLfloat mVelocity = 300.0f;

		SharedObject* mObject_Body = nullptr;
		SharedObject* mObject_Head = nullptr;

		bool mTargetOn = false;

	public:
		Turret(const glm::vec3& position);
		~Turret();

		GLvoid Update();
		GLvoid Draw() const;
		GLvoid Fire();
	};

	vector<Turret*> turrets;
public:
	TurretManager();
	~TurretManager();

	GLvoid Update();
	GLvoid Draw() const;

	GLvoid Create(const glm::vec3& position);
};