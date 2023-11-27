#pragma once
#include "stdafx.h"
#include "Object.h"

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
		glm::vec3 GetBodyPosition() { return mObject_Body->GetPosition(); };
	};

	vector<Turret*> turrets;
public:
	TurretManager();
	~TurretManager();

	GLvoid Update(SOCKET& client_sock);
	GLvoid Draw() const;

	GLvoid Create(const glm::vec3& position);
};