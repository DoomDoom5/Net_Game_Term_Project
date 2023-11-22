#include "Common.h"
#include <iostream>
#include <cstring>
#include <sstream>

#define SERVERPORT 9000
#define BUFSIZE    1024

using namespace std;

// send
void sendPos(SOCKET& , float& , float& , float& , float , float , float );

// recv
void recvClientInfo(SOCKET& , float& , float& , float& );

int main(int argc, char* argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� ����
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	cout << "���� ���� �Ϸ�!." << endl;

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	while (1) {

		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			addr, ntohs(clientaddr.sin_port));

		// ============ Ŭ���̾�Ʈ�� ������ ��� ==================

		float PlayerX = 0.0f;
		float PlayerY = 0.0f;
		float PlayerZ = 0.0f;

		float DirX = 0.0f;
		float DirY = 0.0f;
		float DirZ = 0.0f;

		while (1) {
		
			/*
			���� ����....
			*/

			// ������ ����
			{
				sendPos(client_sock, PlayerX, PlayerY, PlayerZ, DirX, DirY, DirZ);
			}

			// ������ �۽�
			{
				recvClientInfo(client_sock, DirX, DirY, DirZ);
			}


			Sleep(1000/10);

		}
		// ���� �ݱ�
		closesocket(client_sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			addr, ntohs(clientaddr.sin_port));
	}

	// ���� �ݱ�
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}

void recvClientInfo(SOCKET& client_sock ,float& DirX, float& DirY, float& DirZ)
{
	bool mIsFire = false;
	float Yaw, Pitch = 0;

	char buffer[BUFSIZE];
	int retval = 0;
	retval = recv(client_sock, buffer, BUFSIZE, 0);

	std::istringstream iss(buffer);

	iss >> DirX >> DirY >> DirZ >> mIsFire >> Yaw >> Pitch;

	cout << "���� ���� : < " << DirX << " , " << DirY << " , " << DirZ << " > " << endl
			<< "�߻� ���� : " << mIsFire << endl
			<< " ���콺 ��ǥ : " << Yaw << " , " << Pitch << endl;

}

void sendPos(SOCKET& client_sock, float& playerX, float& playerY, float& playerZ, float mDirX, float mDirY, float mDirZ)
{
	int retval;
	float correction = 1.0f;
	float mSpeed = 50.f;
	// ������ ��ſ� ����� ����
	char buffer[BUFSIZE];

	playerX += mSpeed * mDirX * correction;
	playerZ += mSpeed * mDirZ * correction;

	// glm::vec3�� ���ڿ��� ��ȯ
	std::string vec3AsString =
		std::to_string(playerX) + " " +
		std::to_string(playerY) + " " +
		std::to_string(playerZ);

	// ���ڿ��� C ��Ÿ���� ���ڿ��� ��ȯ
	const char* buf = vec3AsString.c_str();

	// ������ ������
	retval = send(client_sock, buf, (int)strlen(buf), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return;
	}
	printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
}