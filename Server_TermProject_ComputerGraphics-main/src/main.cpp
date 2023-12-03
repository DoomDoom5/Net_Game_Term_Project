#include "stdafx.h"
#include "Object.h"
#include "Player.h"
#include "Timer.h"
#include "Transform.h"
#include "Map.h"
#include "Bullet.h"
#include "Monster.h"
#include "Building.h"
#include "Turret.h"
#include "Wave.h"
#include "Common.h"

char* SERVERIP = (char*)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE 50

const Camera* crntCamera = nullptr;
Camera* cameraMain = nullptr;
Camera* cameraFree = nullptr;
Camera* cameraTop = nullptr;

GLvoid Init();
GLvoid InitMeshes();
GLvoid DrawScene();

GLvoid Update();
GLvoid SendAllPlayersInfo(SOCKET&);

GLvoid ToggleDepthTest();

// values
GLint screenPosX = DEFAULT_SCREEN_POS_X;
GLint screenPosY = DEFAULT_SCREEN_POS_Y;
GLint screenWidth = DEFAULT_SCREEN_WIDTH;
GLint screenHeight = DEFAULT_SCREEN_HEIGHT;
// world
glm::vec3 worldPosition(0.0f, 0.0f, 0.0f);
glm::vec3 worldRotation(0.0f, 0.0f, 0.0f);

// managers
BulletManager* bulletManager = nullptr;
MonsterManager* monsterManager = nullptr;
BuildingManager* buildingManager = nullptr;
TurretManager* turretManager = nullptr;
WaveManager* waveManager = nullptr;

// objects
Map* crntMap = nullptr;
Player* player[3] = { nullptr ,nullptr ,nullptr };

///// [Thread] /////
// ���� ��� ������ �Լ�
DWORD WINAPI SleepCls(LPVOID arg);
DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg); 
vector <string> Current(MAXUSER);
vector <bool> ClientOn(MAXUSER);

struct  USER
{
    SOCKET client_sock = NULL;
    int id;
};
int users = 0;



GLint main(GLint argc, GLchar** argv)
{
	srand((unsigned int)time(NULL));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(DEFAULT_SCREEN_POS_X, DEFAULT_SCREEN_POS_Y);
	glutInitWindowSize(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutCreateWindow("TestProject");
	glewExperimental = GL_TRUE;

	Init();

	glutIdleFunc(Update);
	glutDisplayFunc(DrawScene);
    timer::StartUpdate();

	glutMainLoop();
}


///// INIT /////
MyColor backColor;
GLvoid Init()
{
    glewInit();
    InitMeshes();
    timer::Init();

    waveManager->Start();

	//************ [Server]************

    // ���� ��� ������ ����
    CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);
}
GLvoid InitMeshes()
{
	InitModels();
	InitObject();

	bulletManager = new BulletManager();
	buildingManager = new BuildingManager();
	turretManager = new TurretManager();
	monsterManager = new MonsterManager();
	waveManager = new WaveManager();

    buildingManager->Create(BuildingType::Core, { 0, 0, 550 });
    turretManager->Create(glm::vec3(100,0,450));

	crntMap = new Map();

}
GLvoid Reset()
{
    DeleteObjects();

    delete bulletManager;
    delete monsterManager;
    delete buildingManager;
    delete turretManager;
    delete waveManager;

    bulletManager = nullptr;
    monsterManager = nullptr;
    buildingManager = nullptr;
    turretManager = nullptr;
    waveManager = nullptr;

    delete cameraFree;
    delete cameraTop;
    cameraFree = nullptr;
    cameraTop = nullptr;

    if (crntMap != nullptr)
    {
        delete crntMap;
        crntMap = nullptr;
    }
    for (int i = 0; i < users; ++i)
    {
        if (player[i] != nullptr)
        {
            delete player[i];
            player[i] = nullptr;
        }
    }
    Init();
}


///// Draw /////
GLvoid DrawScene()
{
    glutSwapBuffers();
}

///// [ HANDLE EVENTS ] /////
GLvoid Update()
{

	if (IsGameOver() == GL_TRUE)
	{
	//	glutPostRedisplay();
        return;
	}
    if (player[0] == nullptr) return;

    timer::CalculateFPS();
    timer::Update();

    SetConsoleCursor(0, 1);
    printf("���� ������ �� %d / %d\n", users, MAXUSER);
	monsterManager->Update();

    for (size_t i = 0; i < users; i++)  if (player[i] != nullptr) player[i]->Update();
	//buildingManager->Update();
	//turretManager->Update();
	//waveManager->Update();

    glutPostRedisplay();
}

// TCP ���� ���� �κ�
DWORD WINAPI ServerMain(LPVOID arg)
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
    retval = ::bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");
    printf("���� �غ� �Ϸ� \r\n");

    // ������ ��ſ� ����� ����
    SOCKET client_sock;
    struct sockaddr_in clientaddr;
    int addrlen;
    HANDLE hThread;


    // ȭ�� �ʱ�ȭ ������
    HANDLE h_cThread = CreateThread(NULL, 0, SleepCls,
        NULL, 0, NULL);

    while (1) {
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            printf("accept()");
            break;
        }

        // ������ Ŭ���̾�Ʈ ���� ���
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        printf("\r\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\r\n",
            addr, ntohs(clientaddr.sin_port));

        USER client;
        client.client_sock = client_sock;


        if (!ClientOn[users])// �ڸ��� ��������
        {
            ClientOn[users] = true;
            client.id = users++;
        }


        // ������ ����
        hThread = CreateThread(NULL, 0, ProcessClient,
            (LPVOID)&client, 0, NULL);
        if (hThread == NULL) {
            closesocket(client_sock);
            users--;
        }
        else { CloseHandle(hThread); }

    }

    // ���� �ݱ�
    closesocket(listen_sock);

    // ���� ����
    WSACleanup();
    return 0;
}

// ȭ�� �ʱ�ȭ
DWORD WINAPI SleepCls(LPVOID arg)
{
    while (true)
    {
        Sleep(2000);
        SetConsoleCursor(0, 1);
        printf("���� ������ �� %d / %d\n", users, MAXUSER);
    }
}



// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI ProcessClient(LPVOID arg)
{
    int retval;
    USER* usr = (USER*)arg;
    int id = usr->id;

    SOCKET player_sock = usr->client_sock;
    struct sockaddr_in Tclientaddr;
    char addr[INET_ADDRSTRLEN];
    int Taddrlen;

    // Ŭ���̾�Ʈ ���� ���
    Taddrlen = sizeof(Tclientaddr);
    getpeername(player_sock, (struct sockaddr*)&Tclientaddr, &Taddrlen);
    inet_ntop(AF_INET, &Tclientaddr.sin_addr, addr, sizeof(addr));

    player[id] = new Player({ 0,0,0 });
    monsterManager->SetPlayer(player[id]);
    waveManager->SetPlayer(player[id]);



    while (1)
    {
        
        monsterManager->SendBuf(player_sock);

        player[id]->PlayerRecv(player_sock);
        player[id]->PlayerSend(player_sock);

        // ====================================
        SendAllPlayersInfo(player_sock);
        // ====================================

        //Sleep(1000/60);
    }
    /*
    send/recv ����  [�� ��ų��!]

    1. player[0]->Update(client_sock()); -> Ŭ�󿡼� ��ȯ�� �κ� ����
    2.	bulletManager->send(client_sock);
    3.
    4.	buildingManager->send(client_sock);
    5.	turretManager->send(client_sock);
    6.	waveManager->send(client_sock);
    7. player[0]->recv(client_sock(); -> �÷��̾� ��ȭ�� �κ� Ŭ�󿡰� ����

    */
} 

struct PlayersInfo
{
    char num[sizeof(int)];
    char pos[sizeof(uint32_t) * 3 * MAXUSER];
    char look[sizeof(uint32_t) * 3 * MAXUSER];
};

GLvoid SendAllPlayersInfo(SOCKET& sock)
{
    PlayersInfo playersInfo;
    char buf[sizeof(PlayersInfo)];

    memcpy(playersInfo.num, &users, sizeof(int));
    
    uint32_t nPos[MAXUSER * 3]; // �ִ� 3�� �÷��̾� xyz(3) ����
    uint32_t nLook[MAXUSER * 3]; // �ִ� 3�� �÷��̾� xyz(3) ����

    cout << "SendToClient: " << endl;
    for (size_t i = 0; i < users; i++)
    {
        glm::vec3 playerPos = player[i]->GetPosition();
        glm::vec3 playerLook = player[i]->GetLook();
        nPos[i * 3 + 0] = *reinterpret_cast<uint32_t*>(&playerPos.x);
        nPos[i * 3 + 1] = *reinterpret_cast<uint32_t*>(&playerPos.y);
        nPos[i * 3 + 2] = *reinterpret_cast<uint32_t*>(&playerPos.z);
        nLook[i * 3 + 0] = *reinterpret_cast<uint32_t*>(&playerLook.x);
        nLook[i * 3 + 1] = *reinterpret_cast<uint32_t*>(&playerLook.y);
        nLook[i * 3 + 2] = *reinterpret_cast<uint32_t*>(&playerLook.z);
        cout << i << " Pos: (" << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << ")" << endl;
        cout << i << " Look: (" << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << ")" << endl;
    }
    memcpy(playersInfo.pos, nPos, sizeof(uint32_t) * 3 * users);
    memcpy(playersInfo.look, nLook, sizeof(uint32_t) * 3 * users);
    memcpy(buf, &playersInfo, sizeof(PlayersInfo));
    send(sock, buf, sizeof(PlayersInfo), 0);
}