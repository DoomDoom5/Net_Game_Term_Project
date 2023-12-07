#pragma once
#include <winsock2.h> // ����2 ���� ���
#include <ws2tcpip.h> // ����2 Ȯ�� ���
#include "stdafx.h"

class Player;

class WaveManager {
private:
	GLint mCrntWave = 0;
	Player* mPlayer[3] = { nullptr,nullptr,nullptr };

	glm::vec2 GetRandomMonsterPos(const GLint& mapWidth, const GLfloat& mapTop);
public:
	WaveManager();

	GLvoid Start();
	GLvoid Update();

	inline constexpr GLvoid AddPlayer(Player* player, int n) { mPlayer[n] = player; }
	inline constexpr GLvoid DeletePlayer(int n) { mPlayer[n] = nullptr; }

	inline constexpr GLint GetWave() const { return mCrntWave; }

	GLvoid SendBuf(SOCKET& client_sock) { send(client_sock, (const char*)mCrntWave, sizeof(int), 0); };
};