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

GLvoid TurretManager::Update(SOCKET& sock)
{
	printf("\Turret 업데이트 진입\n");

	char numbuf[512] = { 0 };
	int retval = 0;
	retval = recv(sock, numbuf, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		printf("SOCKET_ERROR\n");
		return;
	}
	int num = atoi(numbuf);
	printf("%d개의 데이터를 받을게요\n", num);
	char buffer[2000];
	float recvv3[1000] = {};
	
	// 데이터 받기
	retval = recv(sock, buffer, 2000, 0);
	if (retval == SOCKET_ERROR) {
		printf("SOCKET_ERROR\n");
		return;
	}

	std::stringstream ss(buffer);
	std::string token;
	int cnt = 0;
	float currentValue;
	while (ss >> currentValue) {
		recvv3[cnt++] = currentValue;
	}
	// 받은 데이터를 출력
	for (int i = 0; i < num; ++i) {
		std::cout << i << ": (" << recvv3[3 * i + 0] << ", " << recvv3[3 * i + 1] << ", " << recvv3[3 * i + 2] << ")\n";
	}
	int cnt2 = 0;
	for (auto it = turrets.begin(); it != turrets.end();)
	{
		Turret* turret = *it;
		turret->SetPosition(recvv3[3 * cnt2 + 0], 0, recvv3[3 * cnt2 + 2]);
		++it;
		++cnt2;
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