#include "stdafx.h"
#include "Monster.h"
#include "Object.h"
#include "Player.h"
#include "Timer.h"
#include "Building.h"
#include "Sound.h"

#define MIN_DISTANCE 10 //PLAYER_RADIUS

extern BulletManager* bulletManager;
extern BuildingManager* buildingManager;
extern SoundManager* soundManager;

unordered_map<MonsterType, Textures> modelMap{
	{MonsterType::Blooper, Textures::Blooper},
	{MonsterType::Egg, Textures::Egg},
	{MonsterType::Koromon, Textures::Koromon},
};

Monster::Monster(const MonsterType& monsterType, const glm::vec3& position)
{
	if (modelMap.find(monsterType) == modelMap.end())
	{
		assert(0);
	}

	mCollisionType = CollisionType::Circle;

	const ModelObject* modelObject = GetIdentityTextureObject(modelMap[monsterType]);
	mObject = new SharedObject(modelObject);
	mObject->SetPosition(position);

	switch (monsterType)
	{
	case MonsterType::Blooper:
		mType = monsterType;
		mHP = 100.0f;
		mSpeed = 30.0f;
		mDetectRadius = 200.0f;
		mDamage = 10.0f;
		break;
	case MonsterType::Egg:
		mType = monsterType;
		mHP = 50.0f;
		mSpeed = 40.0f;
		mDetectRadius = 100.0f;
		mDamage = 5.0f;
		break;
	case MonsterType::Koromon:
		mType = monsterType;
		mHP = 150.0f;
		mSpeed = 150.0f;
		mDetectRadius = 150.0f;
		mDamage = 15.0f;
		break;
	default:
		assert(0);
		break;
	}

	GLfloat modelWidth = modelObject->GetWidth();
	GLfloat modelDepth = modelObject->GetDepth();
	mHeight = modelObject->GetHeight();

	(modelWidth > modelDepth) ? mRadius = modelWidth : mRadius = modelDepth;
	mRadius /= 2;


	bulletManager->AddCollisionObject(this);
}

GLvoid Monster::Look(const glm::vec3* target)
{
	glm::vec3 monsterPos = mObject->GetPosition();
	glm::vec3 v = { target->x, monsterPos.y, target->z };
	glm::vec3 u = monsterPos;

	glm::vec3 look = glm::normalize(v - u);
	mObject->SetLook(look);
}
GLvoid Monster::Update(const glm::vec3* target)
{
	mCrntAttackDelay += timer::DeltaTime();

	if (target == nullptr)
	{
		return;
	}
	Look(target);

	GLfloat length = glm::length(ConvertVec2(mObject->GetPosition()) - ConvertVec2(*target));
	if (length > MIN_DISTANCE)
	{
		mObject->MoveZ(mSpeed);
	}
}
GLvoid Monster::Draw() const
{
	mObject->Draw();
}

GLboolean Monster::CheckCollisionBullet(const BulletAtt& bullet, glm::vec3& hitPoint, glm::vec3& normal)
{
	if (bullet.damage <= 0)
	{
		return GL_FALSE;
	}

	glm::vec3 monsterPos = mObject->GetPosition();
	GLfloat radius = mObject->GetRadius();
	/* x로 충돌이 없을 경우 */
	if (fabs(monsterPos.x - bullet.crntPos.x) > radius + bullet.radius)
	{
		return GL_FALSE;
	}

	/* y로 충돌이 없을 경우 */
	if (bullet.crntPos.y > monsterPos.y + mObject->GetHeight())
	{
		return GL_FALSE;
	}

	switch (mCollisionType)
	{
	case CollisionType::Circle:
	{
		if (::CheckCollision(monsterPos, bullet.crntPos, mRadius, bullet.radius, mHeight) == GL_TRUE)
		{
			Damage(bullet.damage);
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
glm::vec3 Monster::GetPosition() const
{
	return mObject->GetPosition();
}
glm::vec3 Monster::GetCenter() const
{
	glm::vec3 pos = mObject->GetPosition();
	return glm::vec3(pos.x, pos.y + mObject->GetHeight() / 2.0f, pos.z);
}
GLvoid Monster::SetPosition(float x, float y, float z)
{
	glm::vec3 v = glm::vec3(x, y, z);
	mObject->SetPosition(v);
}
GLvoid Monster::Damage(const GLfloat& damage)
{
	mHP -= damage;
	if (mHP <= 0)
	{
		switch (mType)
		{
		case MonsterType::Blooper:
			soundManager->PlayEffectSound(EffectSound::M_BlooperDead, mObject->GetPosition(), 0.5f);
			break;
		case MonsterType::Egg:
			soundManager->PlayEffectSound(EffectSound::M_EggDead, mObject->GetPosition());
			break;
		case MonsterType::Koromon:
			soundManager->PlayEffectSound(EffectSound::M_KoromonDead, mObject->GetPosition());
			break;
		default:
			break;
		}

		Destroy();

		bulletManager->CreateExplosion(mExplosionColor, mObject->GetCenterPos(), mRadius);
	}
}
GLvoid Monster::Attack(Player* player)
{
	mCrntAttackDelay = 0;
	player->Damage(mDamage);
}
GLvoid Monster::Attack(Building* building)
{
	building->Damage(mDamage);
}

GLvoid Floatable::InitFloat(SharedObject* object, const GLfloat& floatingSpeed, const GLfloat& floatingRange, const GLfloat& floatingOrigin)
{
	mFloatingObject = object;
	mFloatingSpeed = floatingSpeed;
	mFloatingRange = floatingRange;
	mFloatingOrigin = floatingOrigin;
}
GLvoid Floatable::UpdateFloat()
{
	mFloatingObject->MoveY(mFloatingDir * mFloatingSpeed);
	if (mFloatingDir == DOWN)
	{
		if (mFloatingObject->GetPosition().y <= mFloatingOrigin - mFloatingRange)
		{
			mFloatingDir *= -1;
		}
	}
	else
	{
		if (mFloatingObject->GetPosition().y >= mFloatingOrigin + mFloatingRange)
		{
			mFloatingDir *= -1;
		}
	}
}



Blooper::Blooper(const MonsterType& monsterType, const glm::vec3& position) : Monster(monsterType, position)
{
	mExplosionColor = GRAY;

	InitFloat(mObject, 5.0f, 5.0f, position.y);
}
Blooper::~Blooper()
{
}
GLvoid Blooper::Update(const glm::vec3* target)
{
	Monster::Update(target);
	UpdateFloat();
}



Egg::Egg(const MonsterType& monsterType, const glm::vec3& position) : Monster(monsterType, position)
{
	mExplosionColor = AQUA;

	GLfloat randRotation = static_cast<GLfloat>(rand() % 360);
	mObject->RotateModel(Vector3::Up(), randRotation);

	InitFloat(mObject, 10.0f, 8.0f, position.y);
}
Egg::~Egg()
{

}
GLvoid Egg::Update(const glm::vec3* target)
{
	Monster::Update(target);
	mObject->RotateModel(Vector3::Up(), timer::DeltaTime() * mRotationPerSec);

	UpdateFloat();
}


Koromon::Koromon(const MonsterType& monsterType, const glm::vec3& position) : Monster(monsterType, position)
{
	mExplosionColor = PINK;
}
Koromon::~Koromon()
{
}
GLvoid Koromon::Update(const glm::vec3* target)
{
	mCrntAttackDelay += timer::DeltaTime();
	mCrntJumpDelay += timer::DeltaTime();

	if (target == nullptr)
	{
		mCrntJumpTime = 0;
		mObject->SetPosY(0.0f);
		return;
	}

	if (mCrntJumpDelay < mJumpDelay)
	{
		return;
	}

	mCrntJumpTime += timer::DeltaTime();
	if (mCrntJumpTime > mJumpTime)
	{
		mCrntJumpDelay = 0;
		mCrntJumpTime = 0;
		mObject->SetPosY(0.0f);
		return;
	}



	constexpr GLfloat yaw = 30.0f;
	constexpr GLfloat weight = 45.0f;
	GLfloat t = mCrntJumpTime;

	Look(target);

	const GLfloat mAngleY = sin(DEGREE_TO_RADIAN(yaw));
	const GLfloat mAngleZ = cos(DEGREE_TO_RADIAN(yaw));

	GLfloat length = glm::length(ConvertVec2(mObject->GetPosition()) - ConvertVec2(*target));
	if (length > MIN_DISTANCE)
	{
		mObject->MoveZ(mSpeed * mAngleZ);
	}
	mObject->MoveY(mSpeed * mAngleY - (0.5f * GRAVITY * t * t * weight));
}




const glm::vec3* MonsterManager::FindTargetPos(const glm::vec3& monsterPos, const GLfloat& radius) const
{
	const glm::vec3* target = nullptr;
	const glm::vec3* corePos = buildingManager->GetCorePos();

	glm::vec2 playerCenter = ConvertVec2(mPlayer->GetPosition());
	glm::vec2 monsterCenter = { monsterPos.x, monsterPos.z };
	GLfloat distanceToPlayer = glm::length(playerCenter - monsterCenter);

	GLfloat distanceToCore = FLOAT_MAX;
	if (corePos != nullptr)
	{
		glm::vec2 coreCenter = ConvertVec2(*corePos);
		distanceToCore = glm::length(coreCenter - monsterCenter);

	}

	if (distanceToPlayer < radius)
	{
		if (distanceToPlayer < distanceToCore)
		{
			target = mPlayer->GetRefPos();
		}
		else
		{
			target = buildingManager->GetCorePos();
		}
	}
	else
	{
		target = buildingManager->GetCorePos();
	}

	return target;
}


MonsterManager::MonsterManager()
{
	mMonsterList.reserve(100);
}
MonsterManager::~MonsterManager()
{
	for (Monster* monster : mMonsterList)
	{
		delete monster;
	}
}
GLvoid MonsterManager::Create(const MonsterType& monsterType, const glm::vec3& position)
{
	Monster* monster = nullptr;
	switch (monsterType)
	{
	case MonsterType::Blooper:
		monster = new Blooper(monsterType, position);
		break;
	case MonsterType::Egg:
		monster = new Egg(monsterType, position);
		break;
	case MonsterType::Koromon:
		monster = new Koromon(monsterType, position);
		break;
	default:
		assert(0);
		break;
	}

	mMonsterList.emplace_back(monster);
}
#define MAX_MONSTERS 100
GLvoid MonsterManager::Update(SOCKET& sock)
{
#ifdef DEBUG
	printf("Monster:\n");
#endif
	MonsterInfo monsterInfo{};
	char buf[sizeof(MonsterInfo)];
	int retval = recv(sock, buf, sizeof(MonsterInfo), 0);
	if (retval == SOCKET_ERROR) {
		printf("SOCKET_ERROR\n");
		return;
	}
	memcpy(&monsterInfo, &buf, sizeof(MonsterInfo));

	int nMonsters = 0;
	memcpy(&nMonsters, &monsterInfo.monsterNumBuf, sizeof(int));
	
	MonsterType types[MAX_MONSTERS];
	memcpy(&types, &monsterInfo.monsterTypeBuf, sizeof(MonsterType) * nMonsters);

	uint32_t nPos[MAX_MONSTERS * 3];
	memcpy(&nPos, &monsterInfo.monsterPosBuf, sizeof(uint32_t) * 3 * nMonsters);
	uint32_t nTarget[MAX_MONSTERS * 3];
	memcpy(&nTarget, &monsterInfo.monsterTargetBuf, sizeof(uint32_t) * 3 * nMonsters);
	
	glm::vec3 fPos[MAX_MONSTERS];
	glm::vec3 fTarget[MAX_MONSTERS];
	for (int i = 0; i < nMonsters; ++i) {
		fPos[i].x = *reinterpret_cast<float*>(&nPos[3 * i + 0]);
		fPos[i].y = *reinterpret_cast<float*>(&nPos[3 * i + 1]);
		fPos[i].z = *reinterpret_cast<float*>(&nPos[3 * i + 2]);
		fTarget[i].x = *reinterpret_cast<float*>(&nTarget[3 * i + 0]);
		fTarget[i].y = *reinterpret_cast<float*>(&nTarget[3 * i + 1]);
		fTarget[i].z = *reinterpret_cast<float*>(&nTarget[3 * i + 2]);

#ifdef DEBUG
		printf("%d Position: %.1f, %.1f, %.1f / ", i, fPos[i].x, fPos[i].y, fPos[i].z);
		switch (types[i]) {
		case MonsterType::Blooper:
			printf("Type: Blooper / ");
			break;
		case MonsterType::Egg:
			printf("Type: Egg / ");
			break;
		case MonsterType::Koromon:
			printf("Type: Koromon / ");
			break;
		case MonsterType::None:
			printf("Type: None / ");
			break;
		};
		printf("Target: %.1f, %.1f, %.1f\n", i, fTarget[i].x, fTarget[i].y, fTarget[i].z);
#endif
	}

	mMonsterList.clear();
	for (int i = 0; i < nMonsters; ++i) {
		Create(types[i], fPos[i]);
		mMonsterList[i]->Look(&fTarget[i]);
	}
}

GLvoid MonsterManager::Draw() const
{
	for (const Monster* monster : mMonsterList)
	{
		monster->Draw();
	}
}

GLvoid MonsterManager::SetPlayer(Player* player)
{
	mPlayer = player;
}

GLboolean MonsterManager::GetShortestMonsterPos(const glm::vec3& srcPos, const GLfloat& radius, glm::vec3& targetPos) const
{
	GLfloat min = radius;

	for (const Monster* monster : mMonsterList)
	{
		glm::vec3 monsterPos = monster->GetPosition();
		GLfloat length = glm::length(monsterPos - srcPos);
		if (length < min)
		{
			min = length;
			targetPos = monster->GetCenter();
		}
	}

	if (min >= radius)
	{
		return GL_FALSE;
	}

	return GL_TRUE;
}

GLvoid MonsterManager::CheckCollision(Monster* monster)
{
	if (monster->CanAttack() == GL_FALSE)
	{
		return;
	}

	glm::vec2 playerCenter = ConvertVec2(mPlayer->GetPosition());
	glm::vec2 monsterCenter = ConvertVec2(monster->GetPosition());
	GLfloat playerRadius = mPlayer->GetRadius();
	GLfloat monsterRadius = monster->GetRadius();

	if (::CheckCollision(playerCenter, monsterCenter, playerRadius, monsterRadius) == GL_TRUE)
	{
		monster->Attack(mPlayer);
		return;
	}

	Building* core = buildingManager->GetCore();
	if (core != nullptr)
	{
		if (::CheckCollision(core->GetBuildingObject()->GetRect(), monsterCenter, monsterRadius) == GL_TRUE)
		{
			monster->Attack(core);
			monster->Destroy();
			return;
		}
	}
}


bool MonsterManager::CheckEnemyEmpty()
{
	return mMonsterList.empty();
}