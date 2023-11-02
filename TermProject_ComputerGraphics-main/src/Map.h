#pragma once
#include "stdafx.h"
#include "Bullet.h"

class ModelObject;
class Circle;

class Map : public IBulletCollisionable {
private:
	ModelObject* mMapObject;
	SharedObject* mCoins = nullptr;

	glm::vec2 mLeftTop = { 0,0 };
	glm::vec2 mLeftBottom = {0,0};
	glm::vec2 mRightBottom = {0,0};
	glm::vec2 mRightTop = {0,0};
public:
	Map();

	GLvoid Draw();
	GLboolean CheckCollision(const Circle* boundingCircle);
	GLboolean CheckCollisionBullet(const BulletAtt& bullet, glm::vec3& hitPoint, glm::vec3& normal);
	inline constexpr GLfloat GetMaxWidth() const { return mRightTop.x - mLeftTop.x; }
	inline constexpr GLfloat GetTop() const { return mRightTop.y; }
};