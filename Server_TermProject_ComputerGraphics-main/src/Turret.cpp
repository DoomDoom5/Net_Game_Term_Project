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

GLvoid TurretManager::Update(SOCKET& client_sock)
{
#ifdef DEBUG
	printf("Turret:");
#endif
	for (Turret* turret : turrets)
	{
		turret->Update();
	}

	TurretInfo turretInfo{};
	int nTurret = turrets.size();
	memcpy(&turretInfo.num, &nTurret, sizeof(int));
	printf("%d\n", nTurret);

	uint32_t nPos[MAX_TURRET * 3];
	uint32_t nLook[MAX_TURRET * 3];
	for (int i = 0; i < nTurret; ++i)
	{
		Turret* turret = turrets[i];
		glm::vec3 pos = turret->GetPosition();
		glm::vec3 look = turret->GetLook();
		nPos[i * 3 + 0] = *reinterpret_cast<uint32_t*>(&pos.x);
		nPos[i * 3 + 1] = *reinterpret_cast<uint32_t*>(&pos.y);
		nPos[i * 3 + 2] = *reinterpret_cast<uint32_t*>(&pos.z);
		nLook[i * 3 + 0] = *reinterpret_cast<uint32_t*>(&look.x);
		nLook[i * 3 + 1] = *reinterpret_cast<uint32_t*>(&look.y);
		nLook[i * 3 + 2] = *reinterpret_cast<uint32_t*>(&look.z);
#ifdef DEBUG
		printf("%d Position: %.1f, %.1f, %.1f / ", i, pos.x, pos.y, pos.z);
		printf("Look: %.1f, %.1f, %.1f\n", look.x, look.y, look.z);
#endif
	}
	memcpy(&turretInfo.look, &nLook, sizeof(uint32_t) * 3 * nTurret);
	memcpy(&turretInfo.pos, &nPos, sizeof(uint32_t) * 3 * nTurret);
}
GLvoid TurretManager::Draw() const
{

}

GLvoid TurretManager::Create(const glm::vec3& position)
{
	Turret* turret = new Turret(position);
	turrets.emplace_back(turret);
}