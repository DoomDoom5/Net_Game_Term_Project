#include "Common.h"
#include <iostream>
#include <cstring>
#include <sstream>

#define SERVERPORT 9000
#define BUFSIZE    512

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

		while (1) {
			{
				system("cls");
				char numbuf[BUFSIZE] = {0};
				retval = 0;
				retval = recv(client_sock, numbuf, sizeof(int), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				int num = atoi(numbuf);
				//memcpy(&num, &numbuf, sizeof(int));
				printf("%d개의 데이터를 받을게요\n", num);
				char buffer[2000];
				float recvv3[1000] = {};
				// 데이터 받기
				retval = 0;
				retval = recv(client_sock, buffer, 2000, 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				//printf("%s", buffer);

				std::stringstream ss(buffer);
				std::string token;
				int i = 0;
				float currentValue;
				while (ss >> currentValue) {
					recvv3[i++] = currentValue;
				}
				// 받은 데이터를 출력
				for (int i = 0; i < num; ++i) {
					std::cout << i << ": (" << recvv3[3*i + 0] << ", " << recvv3[3*i + 1] << ", " << recvv3[3*i + 2] << ")\n";
				}
			}

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