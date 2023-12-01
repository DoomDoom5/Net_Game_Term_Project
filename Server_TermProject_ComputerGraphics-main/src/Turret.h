#pragma once
#include <winsock2.h> // 윈속2 메인 헤더
#include <ws2tcpip.h> // 윈속2 확장 헤더
#include "stdafx.h"
#include "Object.h"

//class SharedObject;

#define MAX_TURRET 100
struct TurretInfo {
	char num[sizeof(int)];
	char look[sizeof(uint32_t) * 3 * MAX_TURRET];
	char pos[sizeof(uint32_t) * 3 * MAX_TURRET];
};

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
		glm::vec3 GetPosition() { return mObject_Body->GetPosition(); }
		glm::vec3 GetLook() { return mObject_Head->GetLook(); }
	};

	char m_cBuf[sizeof(TurretInfo)];
	vector<Turret*> turrets;
public:
	TurretManager();
	~TurretManager();

	GLvoid Update();
	GLvoid Draw() const;

	GLvoid Create(const glm::vec3& position);
	GLvoid SendBuf(const SOCKET& sock) { send(sock, m_cBuf, sizeof(TurretInfo), 0); }
};