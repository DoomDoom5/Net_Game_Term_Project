#pragma once
#include "stdafx.h"

class Player;

class WaveManager {
private:
	GLint mCrntWave = 0;
	Player* mPlayer = nullptr;

	glm::vec2 GetRandomMonsterPos(const GLint& mapWidth, const GLfloat& mapTop);
public:
	WaveManager();

	GLvoid Start();
	GLvoid Update();

	inline constexpr GLvoid SetPlayer(Player* player) { mPlayer = player; }

	inline constexpr GLint GetWave() const { return mCrntWave; }
};