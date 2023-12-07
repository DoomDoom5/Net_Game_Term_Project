#pragma once
#include <winsock2.h> // 윈속2 메인 헤더
#include <ws2tcpip.h> // 윈속2 확장 헤더
#include "stdafx.h"
#include "Bullet.h"

enum class MonsterType { None, Blooper, Egg, Koromon };

class Player;
class Building;
class SharedObject;

class Monster : public IBulletCollisionable {
protected:
	SharedObject* mObject = nullptr;

	GLfloat mHP = 100.0f;
	GLfloat mRadius = 0.0f;
	GLfloat mHeight = 0.0f;
	GLfloat mSpeed = 0.0f;
	GLfloat mDetectRadius = 0.0f;

	GLfloat mDamage = 0.0f;
	GLfloat mAttackDelay = 3.0f;
	GLfloat mCrntAttackDelay = 0.0f;

	MonsterType mType = MonsterType::None;

	CollisionType mCollisionType = CollisionType::None;
	COLORREF mExplosionColor = WHITE;

	const glm::vec3* target = nullptr;

	GLboolean isSpawning = GL_TRUE;

public:
	Monster(const MonsterType& monsterType, const glm::vec3& position);
	virtual ~Monster() = 0 {};
	virtual GLvoid Update(const glm::vec3* target);
	virtual GLvoid Look(const glm::vec3* target);
	GLvoid Draw() const;

	GLboolean CheckCollisionBullet(const BulletAtt& bullet, glm::vec3& hitPoint, glm::vec3& normal);
	glm::vec3 GetPosition() const;
	glm::vec3 GetCenter() const;
	MonsterType GetType() const { return mType; }
	GLvoid Damage(const GLfloat& damage);

	GLvoid Attack(Player* player);
	GLvoid Attack(Building* building);
	inline constexpr GLboolean CanAttack() const { return mCrntAttackDelay >= mAttackDelay; }
	inline GLfloat GetRadius() const { return mObject->GetRadius(); }
	inline constexpr GLfloat GetDetectRadius() const { return mDetectRadius; }
};

class Floatable abstract {
private:
	SharedObject* mFloatingObject = nullptr;
	GLint mFloatingDir = UP;
	GLfloat mFloatingSpeed = 0.0f;
	GLfloat mFloatingRange = 0.0f;
	GLfloat mFloatingOrigin = 0.0f;
public:
	GLvoid InitFloat(SharedObject* object, const GLfloat& floatingSpeed, const GLfloat& floatingRange, const GLfloat& floatingOrigin);
	GLvoid UpdateFloat();
};

class Blooper : public Monster, Floatable {
public:
	Blooper(const MonsterType& monsterType, const glm::vec3& position);
	~Blooper();
	GLvoid Update(const glm::vec3* target) override;
};

class Egg : public Monster, Floatable {
private:
	GLfloat mRotationPerSec = 90.0f;

	
public:
	Egg(const MonsterType& monsterType, const glm::vec3& position);
	~Egg();
	GLvoid Update(const glm::vec3* target) override;
};

class Koromon : public Monster {
private:
	const GLfloat mJumpDelay = 2.0f;
	const GLfloat mJumpTime = 1.0f;
	GLfloat mCrntJumpTime = 0.0f;
	GLfloat mCrntJumpDelay = 0.0f;
public:
	Koromon(const MonsterType& monsterType, const glm::vec3& position);
	~Koromon();
	GLvoid Update(const glm::vec3* target) override;
};

struct MonsterInfo {
	char monsterNumBuf[sizeof(int)];
	char monsterPosBuf[sizeof(float) * 3 * 20];		// num은 10이 최대
	char monsterTypeBuf[sizeof(MonsterType) * 20];
	char monsterTargetBuf[sizeof(float) * 3 * 20];
};

class MonsterManager {
private:
	vector<Monster*> mMonsterList;
	Player* mPlayer[MAXUSER];
	int nPlayer = 0;
	const glm::vec3* FindTargetPos(const glm::vec3& monsterPos, const GLfloat& radius) const;

	char m_cBuf[sizeof(MonsterInfo)];
public:
	MonsterManager();
	~MonsterManager();
	GLvoid Create(const MonsterType& monsterType, const glm::vec3& position);
	GLvoid Update();
	GLvoid Draw() const;
	GLvoid AddPlayer(Player* player, int n);
	GLvoid DeletePlayer(int num);
	GLboolean GetShortestMonsterPos(const glm::vec3& srcPos, const GLfloat& radius, glm::vec3& targetPos) const;
	GLvoid CheckCollision(Monster* monster);
	bool CheckEnemyEmpty();

	GLvoid SendBuf(const SOCKET& sock) { send(sock, m_cBuf, sizeof(MonsterInfo), 0); }
};