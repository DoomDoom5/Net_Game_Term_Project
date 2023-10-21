#include "stdafx.h"
#include "Map.h"
#include "Object.h"

Map::Map()
{
	mMapObject = new ModelObject(GetTextureModel(Textures::Map), Shader::Texture);
	mMapObject->SetTexture(Textures::Map);
	AddObject(Shader::Texture, mMapObject);

	set<glm::vec2, CompareSet> boundingMap = mMapObject->GetBoundings_XZ();
	auto iter = boundingMap.begin();
	mLeftTop = *(iter++);
	mLeftBottom = *(iter++);
	mRightBottom = *(iter++);
	mRightTop = *(iter++);

	extern BulletManager* bulletManager;
	bulletManager->AddCollisionObject(this);
	bulletManager->AddParticleCollision(this);

	mCoins = new SharedObject(GetIdentityTextureObject(Textures::Coin));
	mCoins->SetPosition({ 0, 0, 550 });
	AddObject(Shader::Texture, mCoins);
}

GLvoid Map::Draw()
{
}

GLboolean Map::CheckCollision(const Circle* boundingCircle)
{
	glm::vec2 center = boundingCircle->GetCenter();
	GLfloat radius = boundingCircle->GetRadius();

	if (center.y - radius <= mLeftTop.y)
	{
		return true;
	}
	if (center.y + radius >= mLeftBottom.y)
	{
		return true;
	}
	if (center.x - radius < mLeftBottom.x)
	{
		if (::CheckCollision(mLeftTop, mLeftBottom, center, radius) == GL_TRUE)
		{
			return true;
		}
	}
	else if (center.x + radius > mRightBottom.x)
	{
		if (::CheckCollision(mRightTop, mRightBottom, center, radius) == GL_TRUE)
		{
			return true;
		}
	}

	return false;
}

GLboolean Map::CheckCollisionBullet(const BulletAtt& bullet, glm::vec3& hitPoint, glm::vec3& normal)
{
	constexpr GLfloat NO_NORMAL = 9;
	if (bullet.crntPos.y < bullet.radius)
	{
		hitPoint = { bullet.prevPos.x, 0, bullet.prevPos.z };
		normal = Vector3::Up();
		return true;
	}

	// check collision with previous to current bullet position line

	const glm::vec2 center = ConvertVec2(bullet.crntPos);
	const glm::vec2 prevCenter = ConvertVec2(bullet.prevPos);
	glm::vec2 intersection = { 0,0 };

	if (center.y - bullet.radius <= mLeftTop.y)
	{
		intersection = ::GetLineIntersection(mLeftTop, mRightTop, prevCenter, center);
		normal = Vector3::Front();
	}
	else if (center.y + bullet.radius >= mLeftBottom.y)
	{
		intersection = ::GetLineIntersection(mLeftBottom, mRightBottom, prevCenter, center);
		normal = Vector3::Back();
	}
	else if (center.x - bullet.radius < mLeftBottom.x)
	{
		if (::CheckCollision(mLeftTop, mLeftBottom, prevCenter, center) == GL_TRUE)
		{
			intersection = ::GetLineIntersection(mLeftTop, mLeftBottom, prevCenter, center);
			glm::vec3 v = ConvertVec3(mLeftTop - mLeftBottom);
			normal = glm::normalize(glm::cross(v, Vector3::Up()));
		}
		else
		{
			return false;
		}
	}
	else if (center.x + bullet.radius > mRightBottom.x)
	{
		if (::CheckCollision(mRightTop, mRightBottom, prevCenter, center) == GL_TRUE)
		{
			intersection = ::GetLineIntersection(mRightTop, mRightBottom, prevCenter, center);
			glm::vec3 v = ConvertVec3(mRightBottom - mRightTop);
			normal = glm::normalize(glm::cross(v, Vector3::Up()));
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	hitPoint = glm::vec3(intersection.x, bullet.prevPos.y, intersection.y);
	if (bullet.crntPos.y > mMapObject->GetHeight())
	{
		normal.x = NO_NORMAL;
	}
	return true;
}