#pragma once
#include "stdafx.h"
#include "Bullet.h"

class SharedObject;

enum class BuildingType { Core };

class Building : public IBulletCollisionable {
private:
	CollisionType mCollisionType = CollisionType::None;
	SharedObject* mObject = nullptr;
	GLrect mRect{};
	GLfloat mRadius = 0.0f;
	GLfloat mHP = 0.0f;
	glm::vec2 mCenter = glm::vec2(0, 0);

	COLORREF mExplosionColor = WHITE;

public:
	Building(const BuildingType& type, const glm::vec3& position, const glm::vec3 look);
	~Building();

	GLvoid Update();
	GLvoid Draw() const;

	GLboolean CheckCollision(const Circle* boundingCircle) const;
	GLboolean CheckCollisionBullet(const BulletAtt& bullet, glm::vec3& hitPoint, glm::vec3& normal);
	SharedObject* GetBuildingObject() const { return mObject; }
	GLvoid Damage(const GLfloat& damage);
};

class BuildingManager {
private:
	vector<Building*> buildings;
	Building* mCore = nullptr;

public:
	BuildingManager();
	~BuildingManager();

	GLvoid Update();
	GLvoid Draw() const;

	GLvoid Create(const BuildingType& type, const glm::vec3& position, const glm::vec3 look = Vector3::Front());
	GLboolean CheckCollision(const Circle* boundingCircle) const;
	const glm::vec3* GetCorePos() const;
	inline Building* GetCore() const { return mCore; }
};