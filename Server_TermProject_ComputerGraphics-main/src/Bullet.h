#pragma once
#include "stdafx.h"
#include "Object.h"

enum class BulletType { Normal, Particle_Explosion, Rocket, Sniper};

typedef struct BulletAtt {
	glm::vec3 prevPos = glm::vec3(0, 0, 0);
	glm::vec3 crntPos = glm::vec3(0, 0, 0);
	GLfloat radius = 0.0f;
	GLfloat damage = 0.0f;
}BulletAtt;

typedef struct BulletData {
	BulletType type = BulletType::Normal;
	COLORREF color = WHITE;

	GLfloat weight = 0.0f;
	GLfloat scale = 0.0f;
	GLfloat damage = 0.0f;
	GLfloat velocity = 0.0f;

	Models model;
}BulletData;

class IBulletCollisionable abstract {
private:
	GLint mID = 0;
	GLboolean mIsDestroyed = GL_FALSE;
public:
	virtual GLboolean CheckCollisionBullet(const BulletAtt& bullet, glm::vec3& hitPoint, glm::vec3& normal) abstract;
	GLvoid Destroy();
	inline constexpr GLboolean IsDestroyed() const { return mIsDestroyed; };
	inline constexpr GLvoid SetID(const GLint& id) { mID = id; }
	inline constexpr GLint GetID() { return mID; }
};



class Bullet : public SharedObject {
	BulletType mType = BulletType::Normal;

	/* 무게, 중력의 영향을 받는 정도 (n배) */
	GLfloat mWeight = 0.0f;

	GLfloat mDamage = 0.0f;

	/* 이전 좌표값 */
	glm::vec3 mPrevPos = glm::vec3(0, 0, 0);
	/* 시작 좌표값 */
	glm::vec3 mOrigin = glm::vec3(0, 0, 0);

	/* 발사 후 경과한 시간 */
	GLfloat mT = 0.0f;

	/* 탄퍼짐 */
	GLint mSpreadAmount = 2;

	GLfloat mAngleX = 0.0f;
	GLfloat mAngleY = 0.0f;
	GLfloat mAngleZ = 0.0f;
	GLfloat mVelocity = 0.0f;

	GLboolean mDestroyed = GL_FALSE;
public:
	Bullet(const BulletData& data, const glm::vec3& origin, const glm::vec3& position, const GLfloat& yaw, const GLfloat& pitch);
	~Bullet();
	GLvoid Update();
	inline constexpr GLvoid Destroy() { mDestroyed = GL_TRUE; }
	inline constexpr GLboolean IsDestroyed() const { return mDestroyed; }

	BulletAtt GetAttribute() const;
	COLORREF GetColor() const;
	inline constexpr BulletType GetType() const { return mType; }
};

class BulletManager {
private:
	GLint mID = 0;
	vector<Bullet*> mBulletList;
	vector<Bullet*> mParticles;
	vector<PaintPlane*> mPaints;
	vector<IBulletCollisionable*> mCollisionObjectList;
	vector<IBulletCollisionable*> mParticleCollisions;

	GLfloat mCrntInkSoundDelay = 0.0f;
public:
	BulletManager();
	~BulletManager();
	GLvoid Create(const BulletData& data, const glm::vec3& origin, const glm::vec3& position, const GLfloat& yaw, const GLfloat& pitch);
	GLvoid CreateExplosion(const COLORREF& color, const glm::vec3& position, const GLfloat& radius, const GLint& amount = 20);
	GLvoid Draw() const;
	GLvoid Update();

	/* Read only */
	inline constexpr const vector<PaintPlane*>& GetPaints() const { return mPaints; }

	GLvoid AddCollisionObject(IBulletCollisionable* object);
	GLvoid AddParticleCollision(IBulletCollisionable* object);
	GLvoid DelCollisionObject(IBulletCollisionable* object);
	GLvoid DelParticleCollision(IBulletCollisionable* object);
};