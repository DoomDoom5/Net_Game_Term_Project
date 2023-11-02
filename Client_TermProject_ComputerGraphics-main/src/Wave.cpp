#include "stdafx.h"
#include "Wave.h"
#include "Monster.h"
#include "Map.h"
#include "Player.h"

extern MonsterManager* monsterManager;
extern Map* crntMap;

WaveManager::WaveManager()
{
	mCrntWave = 1;
}

glm::vec2 WaveManager::GetRandomMonsterPos(const GLint& mapWidth, const GLfloat& mapTop)
{
	GLfloat posX = rand() % mapWidth - (mapWidth / 2.0f);
	GLfloat posZ = rand() % 50 + mapTop;

	return glm::vec2(posX, posZ);
}

GLvoid WaveManager::Start()
{
	GLint mapWidth = static_cast<GLint>(crntMap->GetMaxWidth());
	GLfloat mapTop = crntMap->GetTop();

	const GLuint numOfBlooper = mCrntWave * 2;
	const GLuint numOfEgg = mCrntWave * 1;
	const GLuint numOfKoromon = mCrntWave * 1;

	for (GLuint i = 0; i < numOfBlooper; ++i)
	{
		glm::vec2 pos = GetRandomMonsterPos(mapWidth, mapTop);
		monsterManager->Create(MonsterType::Blooper, glm::vec3(pos.x, 20, pos.y));
	}

	for (GLuint i = 0; i < numOfEgg; ++i)
	{
		glm::vec2 pos = GetRandomMonsterPos(mapWidth, mapTop);
		monsterManager->Create(MonsterType::Egg, glm::vec3(pos.x, 40, pos.y));
	}

	for (GLuint i = 0; i < numOfKoromon; ++i)
	{
		glm::vec2 pos = GetRandomMonsterPos(mapWidth, mapTop);
		monsterManager->Create(MonsterType::Koromon, glm::vec3(pos.x, 0, pos.y));
	}
}

GLvoid WaveManager::Update()
{
	if (monsterManager->CheckEnemyEmpty())
	{
		mCrntWave++;
		mPlayer->AddHoldturret(1);

		WaveManager::Start();
		cout << mCrntWave << endl;
	}
}
