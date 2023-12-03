#pragma once
#include <winsock2.h> // ����2 ���� ���
#include <ws2tcpip.h> // ����2 Ȯ�� ���
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