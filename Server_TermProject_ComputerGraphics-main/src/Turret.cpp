#pragma once
#include "stdafx.h"
#include "Turret.h"
#include "Object.h"
#include "Bullet.h"
#include "Monster.h"
#include "Timer.h"
#include "Sound.h"

extern MonsterManager* monsterManager;
extern BulletManager* bulletManager;

TurretManager::Turret::Turret(const glm::vec3& position)
{
	mObject_Body = new SharedObject(GetIdentityTextureObject(Textures::Turret_Body));
	mObject_Head = new SharedObject(GetIdentityTextureObject(Textures::Turret_Head));
	mObject_Body->SetPosition(position);
	mObject_Head->SetPosition(position);
	mObject_Head->MoveY(10, GL_FALSE);

	mObject_Body->SetLook(Vector3::Back());
	mObject_Head->SetLook(Vector3::Back());
}
TurretManager::Turret::~Turret()
{

}

GLvoid TurretManager::Turret::Draw() const
{
	mObject_Body->Draw();
	mObject_Head->Draw();
}

GLvoid TurretManager::Turret::Update()
{
	mCrntFireDelay += timer::DeltaTime();

	glm::vec3 targetPos;
	if (monsterManager->GetShortestMonsterPos(mObject_Head->GetPosition(), mRadius, targetPos) == GL_TRUE)
	{
		if (!mTargetOn)
		{
			mTargetOn = true;
		}
		mObject_Head->Look(targetPos);

		if (mCrntFireDelay >= mFireDelay)
		{
			Fire();
		}
	}
	else
	{
		mTargetOn = false;
		mObject_Head->SetLook(Vector3::Back());
	}
}
GLvoid TurretManager::Turret::Fire()
{
	mCrntFireDelay = 0;

	glm::mat4 transform = mObject_Head->GetTransform();
	glm::vec3 originPos = glm::vec3(0, 0, 0);
	glm::vec3 bulletPos = glm::vec3(0, 2, 13);
	MultiplyVector(transform, bulletPos);
	MultiplyVector(transform, originPos);

	GLfloat yaw = 0.0f;
	GLfloat pitch = 0.0f;
	GetYawPitch(mObject_Head->GetLook(), yaw, pitch);

	BulletData data;
	data.type = BulletType::Normal;
	data.color = PINK;
	data.weight = 30.0f;
	data.damage = 20.0f;
	data.scale = 0.1f;
	data.velocity = 300.0f;
	data.model = Models::LowSphere;

	bulletManager->Create(data, originPos, bulletPos, yaw, pitch);
}


TurretManager::TurretManager()
{
	turrets.reserve(20);
}

TurretManager::~TurretManager()
{
	for (Turret* turret : turrets)
	{
		delete turret;
	}
}
struct TurretInfo {
	char TurretNumBuf[sizeof(int)];
	char TurretPosBuf[sizeof(float) * 3 * 20];		// num은 10이 최대
	char TurretTypeBuf[sizeof(int) * 20];
	char TurretTargetBuf[sizeof(float) * 3 * 20];
};

GLvoid TurretManager::Update(SOCKET& client_sock)
{
	printf("\n turret 업데이트 진입\n");
	for (Turret* turret : turrets)
	{
		turret->Update();
	}
	// =================================
	
	TurretInfo turretInfo{};

	char numbuf[10];
	int num = 0;
	turretInfo.TurretTargetBuf;

	int nTurrets = 0;
	int netbyte = 0;
	if (!turrets.empty())
		nTurrets = turrets.size();
	std::cout << nTurrets << "터렛 위치가 있음" << std::endl;
	netbyte = htonl(nTurrets);
	memcpy(&turretInfo.TurretNumBuf, &netbyte, sizeof(int));

	// Postion 설정
	uint32_t converToFloat[1000];
	memset(converToFloat, 0, sizeof(converToFloat));
	for (int i = 0; i < nTurrets; ++i)
	{
		Turret* turret = turrets[i];
		glm::vec3 pos = turret->GetBodyPosition();
		converToFloat[i * 3 + 0] = htonl(*reinterpret_cast<uint32_t*>(&pos.x));
		converToFloat[i * 3 + 1] = htonl(*reinterpret_cast<uint32_t*>(&pos.y));
		converToFloat[i * 3 + 2] = htonl(*reinterpret_cast<uint32_t*>(&pos.z));
		printf("%d Position: (%f, %f, %f)\n", i, pos.x, pos.y, pos.z);
	}
	memcpy(&turretInfo.TurretPosBuf, &converToFloat, sizeof(uint32_t) * 3 * nTurrets);
	
	char buf[sizeof(TurretInfo)];
	memcpy(&buf, &turretInfo, sizeof(TurretInfo));
	send(client_sock, buf, sizeof(turretInfo), 0);
}

GLvoid TurretManager::Create(const glm::vec3& position)
{
	Turret* turret = new Turret(position);
	turrets.emplace_back(turret);
}