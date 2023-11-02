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

GLvoid TurretManager::Update()
{
	for (Turret* turret : turrets)
	{
		turret->Update();
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