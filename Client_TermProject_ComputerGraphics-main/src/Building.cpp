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

GLvoid Building::Update()
{

}


GLboolean Building::CheckCollision(const Circle* boundingCircle) const
{
	glm::vec2 targetCenter = boundingCircle->GetCenter();
	GLfloat targetRadius = boundingCircle->GetRadius();

	if (::CheckCollision(mCenter, targetCenter, mRadius, targetRadius) == GL_FALSE)
	{
		return GL_FALSE;
	}

	if (targetCenter.x + targetRadius < mRect.left)
	{
		return GL_FALSE;
	}
	if (targetCenter.x - targetRadius > mRect.right)
	{
		return GL_FALSE;
	}
	if (targetCenter.y + targetRadius < mRect.top)
	{
		return GL_FALSE;
	}
	if (targetCenter.y - targetRadius > mRect.bottom)
	{
		return GL_FALSE;
	}

	return GL_TRUE;
}
GLboolean Building::CheckCollisionBullet(const BulletAtt& bullet, glm::vec3& hitPoint, glm::vec3& normal)
{
	GLrect rect = mObject->GetRect();
	if (::CheckCollision(ConvertVec2(mObject->GetPosition()), ConvertVec2(bullet.crntPos), rect.GetRadius(), bullet.radius) == GL_FALSE)
	{
		return GL_FALSE;
	}

	switch(mCollisionType)
	{
	case CollisionType::Rect:
	{
		const glm::vec2 bulletCenter = { bullet.crntPos.x, bullet.crntPos.z };
		GLfloat transformedPosY = mObject->GetTransformedPos().y;
		if (::CheckCollision(rect, bulletCenter, bullet.radius) == GL_TRUE &&
			bullet.crntPos.y - bullet.radius <= transformedPosY + mObject->GetHeight() &&
			bullet.crntPos.y + bullet.radius >= transformedPosY)
		{
			const glm::vec2 prevBulletCenter = { bullet.prevPos.x, bullet.prevPos.z };

			glm::vec2 leftTop = { rect.left, rect.top };
			glm::vec2 leftBottom = { rect.left,rect.bottom };
			glm::vec2 rightTop = { rect.right , rect.top };
			glm::vec2 rightBottom = { rect.right ,rect.bottom };

			vector<Line> lines;
			lines.emplace_back(Line(leftTop, leftBottom));
			lines.emplace_back(Line(leftBottom, rightBottom));
			lines.emplace_back(Line(rightBottom, rightTop));
			lines.emplace_back(Line(rightTop, leftTop));
			for (const Line& line : lines)
			{
				if (::CheckCollision(line.v, line.u, prevBulletCenter, bulletCenter) == GL_TRUE)
				{
					glm::vec2 point = GetLineIntersection(line.v, line.u, prevBulletCenter, bulletCenter);
					hitPoint = { point.x, bullet.prevPos.y, point.y };
					if (line.v == leftTop)
					{
						normal = Vector3::Right();
					}
					else if (line.v == leftBottom)
					{
						normal = Vector3::Front();
					}
					else if (line.v == rightBottom)
					{
						normal = Vector3::Left();
					}
					else
					{
						normal = Vector3::Back();
					}

					return GL_TRUE;
				}
			}

			/* up-down side */
			if (bullet.prevPos.y > transformedPosY + mObject->GetHeight())
			{
				hitPoint = { bullet.prevPos.x, transformedPosY + mObject->GetHeight(), bullet.prevPos.z };
				normal = Vector3::Up();
			}
			else if (bullet.prevPos.y < transformedPosY)
			{
				hitPoint = { bullet.prevPos.x, transformedPosY, bullet.prevPos.z };
				normal = Vector3::Down();
			}
			return GL_TRUE;
		}
	}
		break;
	default:
		assert(0);
		break;
	}

	return GL_FALSE;
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

GLvoid BuildingManager::Update()
{
	for (auto it = buildings.begin(); it != buildings.end();)
	{
		Building* building = *it;
		if (building != nullptr && building->IsDestroyed() == GL_TRUE)
		{
			it = buildings.erase(it);
			if (building == mCore)
			{
				DeleteBlendObject(building->GetBuildingObject());
				delete mCore;
				mCore = nullptr;
			}
		}
		else
		{
			building->Update();

			++it;
		}
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

GLboolean BuildingManager::CheckCollision(const Circle* boundingCircle) const
{
	for (const Building* building : buildings)
	{
		if (building->CheckCollision(boundingCircle) == GL_TRUE)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

const glm::vec3* BuildingManager::GetCorePos() const
{
	if (mCore == nullptr)
	{
		return nullptr;
	}

	return mCore->GetBuildingObject()->GetRefPos();
}