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