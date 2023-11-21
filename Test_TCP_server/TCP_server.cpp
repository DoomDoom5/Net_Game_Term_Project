#include "Common.h"
#include <iostream>
#include <cstring>
#include <sstream>

#define SERVERPORT 9000
#define BUFSIZE    1024

using namespace std;

// send
void sendPos(SOCKET& client_sock);

// recv
void recvClientInfo(SOCKET& client_sock);

int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
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

	cout << "서버 생성 완료!." << endl;

	// 데이터 통신에 사용할 변수
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

		// 접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			addr, ntohs(clientaddr.sin_port));

		// ============ 클라이언트와 데이터 통신 ==================
		
		while (1) {
		
			/*
			게임 연산....
			*/

			// 데이터 수신
			{
			//	sendPos(client_sock);
			}

			// 데이터 송신
			{
				recvClientInfo(client_sock);
			}


			Sleep(1000/10);

		}
		// 소켓 닫기
		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
			addr, ntohs(clientaddr.sin_port));
	}

	// 소켓 닫기
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

void recvClientInfo(SOCKET& client_sock)
{
	char DirX, DirY, DirZ = 0;
	bool mIsFire = false;
	float Yaw, Pitch = 0;

	char buffer[BUFSIZE];
	int retval = 0;
	retval = recv(client_sock, buffer, BUFSIZE, 0);

	std::istringstream iss(buffer);

	iss >> DirX >> DirY >> DirZ >> mIsFire >> Yaw >> Pitch;

	cout << "방향 벡터 : < " << DirX << " , " << DirY << " , " << DirZ << " > " << endl
			<< "발사 유무 : " << mIsFire << endl
			<< " 마우스 좌표 : " << Yaw << " , " << Pitch << endl;

}

void sendPos(SOCKET& client_sock)
{
	char buffer[BUFSIZE];
	float PlayerX = 0.0f;
	float PlayerY = 0.0f;
	float PlayerZ = 0.0f;

	int retval;

	// 데이터 통신에 사용할 변수
	PlayerX += 0.2f;
	// glm::vec3를 문자열로 변환
	std::string vec3AsString =
		std::to_string(PlayerX) + " " +
		std::to_string(PlayerY) + " " +
		std::to_string(PlayerZ);

	// 문자열을 C 스타일의 문자열로 변환
	const char* buf = vec3AsString.c_str();

	// 데이터 보내기
	retval = send(client_sock, buf, (int)strlen(buf), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return;
	}
	printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);
}