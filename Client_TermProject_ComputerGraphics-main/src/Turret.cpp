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
extern SoundManager* soundManager;

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

GLvoid TurretManager::Turret::SetLook(const glm::vec3& look)
{
	mObject_Head->SetLook(look);
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
			soundManager->PlayEffectSound(EffectSound::Turret_FindEnemy, mObject_Head->GetPosition(), 0.1f);
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

	soundManager->PlayEffectSound(EffectSound::Normal_shot, mObject_Head->GetPosition(), 0.1f);

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

GLvoid TurretManager::Update(const SOCKET& sock)
{
#ifdef DEBUG
	printf("Turret: ");
#endif
	TurretInfo turretInfo{};
	char buf[sizeof(TurretInfo)];
	int retval = recv(sock, buf, sizeof(TurretInfo), 0);
	if (retval == SOCKET_ERROR) {
		printf("SOCKET_ERROR\n");
		return;
	}
	memcpy(&turretInfo, &buf, sizeof(TurretInfo));

	int nTurrets = 0;
	memcpy(&nTurrets, &turretInfo.num, sizeof(int));
	printf("%d\n", nTurrets);

	uint32_t nPos[MAX_TURRET * 3];
	memcpy(&nPos, &turretInfo.pos, sizeof(uint32_t) * 3 * nTurrets);
	uint32_t nLook[MAX_TURRET * 3];
	memcpy(&nLook, &turretInfo.look, sizeof(uint32_t) * 3 * nTurrets);

	glm::vec3 fPos[MAX_TURRET];
	glm::vec3 fLook[MAX_TURRET];
	for (int i = 0; i < nTurrets; ++i) {
		fPos[i].x = *reinterpret_cast<float*>(&nPos[3 * i + 0]);
		fPos[i].y = *reinterpret_cast<float*>(&nPos[3 * i + 1]);
		fPos[i].z = *reinterpret_cast<float*>(&nPos[3 * i + 2]);
		fLook[i].x = *reinterpret_cast<float*>(&fLook[3 * i + 0]);
		fLook[i].y = *reinterpret_cast<float*>(&fLook[3 * i + 1]);
		fLook[i].z = *reinterpret_cast<float*>(&fLook[3 * i + 2]);
#ifdef DEBUG
		printf("%d Position: %.1f, %.1f, %.1f / ", i, fPos[i].x, fPos[i].y, fPos[i].z);
		printf("Look: %.1f, %.1f, %.1f\n", fLook[i].x, fLook[i].y, fLook[i].z);
#endif
	}

	turrets.clear();
	for (int i = 0; i < nTurrets; ++i) {
		Create(fPos[i]);
		turrets[i]->SetLook(fLook[i]);
	}
}

GLvoid TurretManager::Draw() const
{
	for (const Turret* turret : turrets)
	{
		turret->Draw();
	}
}

GLvoid TurretManager::Create(const glm::vec3& position)
{
	Turret* turret = new Turret(position);
	soundManager->PlayEffectSound(EffectSound::Turret_install, position, 0.1f);
	turrets.emplace_back(turret);
}