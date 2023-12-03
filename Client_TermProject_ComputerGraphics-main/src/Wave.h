#pragma once
#include <winsock2.h> // 윈속2 메인 헤더
#include <ws2tcpip.h> // 윈속2 확장 헤더
#include "stdafx.h"

class Player;

class WaveManager {
private:
	GLint mCrntWave = 0;
	Player* mPlayer = nullptr;

	glm::vec2 GetRandomMonsterPos(const GLint& mapWidth, const GLfloat& mapTop);
	string Log;
public:
	WaveManager();

	GLvoid Start();
	GLvoid Update(SOCKET& sock);

	inline constexpr GLvoid SetPlayer(Player* player) { mPlayer = player; }

	inline constexpr GLint GetWave() const { return mCrntWave; }
};