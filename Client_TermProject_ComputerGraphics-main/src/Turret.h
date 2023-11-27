#pragma once
#include "stdafx.h"
#include "Object.h"
#include <winsock2.h> // 윈속2 메인 헤더
#include <ws2tcpip.h> // 윈속2 확장 헤더

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
		GLvoid SetPosition(float x, float y, float z) { mObject_Body->SetPosition(glm::vec3(x,y,z)); };

	};

	vector<Turret*> turrets;
public:
	TurretManager();
	~TurretManager();

	GLvoid Update(SOCKET& sock);
	GLvoid Draw() const;

	GLvoid Create(const glm::vec3& position);
};