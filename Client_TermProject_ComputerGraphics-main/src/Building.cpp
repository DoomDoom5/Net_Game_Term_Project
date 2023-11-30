#pragma once
#include "stdafx.h"
#include "Building.h"

extern BulletManager* bulletManager;


unordered_map<BuildingType, Textures> modelMap{
	{BuildingType::Core, Textures::Core}
};

Building::Building(const BuildingType& type, const glm::vec3& position, const glm::vec3 look)
{
	switch (type)
	{
	case BuildingType::Core:
		mHP = 100.0f;
		mExplosionColor = BLUE;
		break;
	}
	mCollisionType = CollisionType::Rect;
	mObject = new SharedObject(GetIdentityTextureObject(modelMap[type]));
	mObject->SetPosition(position);
	mObject->SetLook(look);

	mRect = mObject->GetRect();
	mCenter = mRect.GetCenter();
	mRadius = mRect.GetRadius();

	if (type == BuildingType::Core)
	{
		AddBlendObject(mObject);
	}

	extern BulletManager* bulletManager;
	bulletManager->AddCollisionObject(this);
	bulletManager->AddParticleCollision(this);
}
Building::~Building()
{

}

GLvoid Building::Draw() const
{
}

GLvoid Building::Damage(const GLfloat& damage)
{
	mHP -= damage;
	if (mHP <= 0)
	{
		GameOver();
		Destroy();
		bulletManager->DelParticleCollision(this);
		bulletManager->CreateExplosion(mExplosionColor, mObject->GetCenterPos(), mObject->GetRadius(), 50);
	}
}





BuildingManager::BuildingManager()
{
	buildings.reserve(20);
}

BuildingManager::~BuildingManager()
{
	for (Building* building : buildings)
	{
		delete building;
	}
}

GLvoid BuildingManager::Update(SOCKET& sock)
{
	printf("Building:\n");
	BuildingInfo buildingInfo{};
	char buf[sizeof(BuildingInfo)];

	int retval = recv(sock, buf, sizeof(BuildingInfo), 0);
	if (retval == SOCKET_ERROR) {
		printf("SOCKET_ERROR\n");
		return;
	}
	memcpy(&buildingInfo, &buf, sizeof(BuildingInfo));
	int nBuilding = 0;
	memcpy(&nBuilding, &buildingInfo.numBuf, sizeof(int));

	uint32_t nPos[100 * 3];
	memcpy(&nPos, &buildingInfo.posBuf, sizeof(uint32_t) * 3 * nBuilding);

	glm::vec3 fPos[100];
	for (int i = 0; i < nBuilding; ++i) {
		fPos[i].x = *reinterpret_cast<float*>(&nPos[3 * i + 0]);
		fPos[i].y = *reinterpret_cast<float*>(&nPos[3 * i + 1]);
		fPos[i].z = *reinterpret_cast<float*>(&nPos[3 * i + 2]);
	}

	BuildingType types[100];
	memcpy(&types, &buildingInfo.typeBuf, sizeof(BuildingType) * nBuilding);

	buildings.clear();
	for (int i = 0; i < nBuilding; ++i) {
#ifdef DEBUG
		printf("%d Position: %.1f, %.1f, %.1f / ", i, fPos[i].x, fPos[i].y, fPos[i].z);
		switch (types[i]) {
		case BuildingType::Core:
			printf("Type: Core\n");
			break;
		};
#endif
		Create(types[i], fPos[i], { 0, 0, 550 });
	}
}
GLvoid BuildingManager::Draw() const
{
	for (Building* building : buildings)
	{
		building->Draw();
	}
}

GLvoid BuildingManager::Create(const BuildingType& type, const glm::vec3& position, const glm::vec3 look)
{
	Building* building = new Building(type, position, look);
	buildings.emplace_back(building);

	if (type == BuildingType::Core)
	{
		mCore = building;
	}
}

const glm::vec3* BuildingManager::GetCorePos() const
{
	if (mCore == nullptr)
	{
		return nullptr;
	}

	return mCore->GetBuildingObject()->GetRefPos();
}