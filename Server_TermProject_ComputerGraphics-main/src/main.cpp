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
// 소켓 통신 스레드 함수
CRITICAL_SECTION cs; // 임계 영역
DWORD WINAPI SleepCls(LPVOID arg);
DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg); 
vector <string> Current(MAXUSER);
vector <bool> ClientOn(MAXUSER);
bool updateOn = false;


struct  USER
{
    SOCKET client_sock = NULL;
    int id = -1;
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
    InitializeCriticalSection(&cs);

    // 소켓 통신 스레드 생성
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
    for (int i = 0; i < MAXUSER; ++i)
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
    if (users <= 0) return;
	if (IsGameOver() == GL_TRUE)
	{
	//	glutPostRedisplay();
        cout << "게임 오버" << endl;
        DeleteCriticalSection(&cs);
        return;
	}


    EnterCriticalSection(&cs);
    updateOn = false;

    timer::CalculateFPS();
    timer::Update();

    printf("서버 접속자 수 %d / %d\n", users, MAXUSER);

    bulletManager->Update();
    monsterManager->Update();
    for (size_t i = 0; i < MAXUSER; i++)  if (player[i] != nullptr) player[i]->Update();
    buildingManager->Update();
    turretManager->Update();
    waveManager->Update();

#ifdef  DEBUG
    cout << "SendToClient: " << endl;
#endif
    for (size_t i = 0; i < MAXUSER; i++)
    {
        if (player[i] == nullptr) continue;
        glm::vec3 vPos = player[i]->GetPosition();
        glm::vec3 vBodyLook = player[i]->GetBodyLook();
        glm::vec3 vHeadLook = player[i]->GetHeadLook();
        glm::vec3 vLegLLook = player[i]->GetLegLLook();
        glm::vec3 vLegRLook = player[i]->GetLegRLook();
        glm::vec3 vGunLook = player[i]->GetGunLook();
        glm::quat gunRotation = player[i]->GetGunRotation();
        GunType gunType = player[i]->GetGunType();

#ifdef  DEBUG
        cout << i << " is ONair" << endl;
        cout << i << " Pos: (" << vPos.x << ", " << vPos.y << ", " << vPos.z << ")" << endl;
        cout << i << " BodyLook: (" << vBodyLook.x << ", " << vBodyLook.y << ", " << vBodyLook.z << ")" << endl;
        cout << i << " HeadLook: (" << vHeadLook.x << ", " << vHeadLook.y << ", " << vHeadLook.z << ")" << endl;
        cout << i << " GunLook: (" << vGunLook.x << ", " << vGunLook.y << ", " << vGunLook.z << ")" << endl;
        cout << i << " LegLeftLook: (" << vLegLLook.x << ", " << vLegLLook.y << ", " << vLegLLook.z << ")" << endl;
        cout << i << " LegRightLook: (" << vLegRLook.x << ", " << vLegRLook.y << ", " << vLegRLook.z << ")" << endl;
#endif

    }

    updateOn = true;

    LeaveCriticalSection(&cs);

    glutPostRedisplay();
}

// TCP 서버 시작 부분
DWORD WINAPI ServerMain(LPVOID arg)
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
    retval = ::bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");
    printf("서버 준비 완료 \r\n");

    // 데이터 통신에 사용할 변수
    SOCKET client_sock;
    struct sockaddr_in clientaddr;
    int addrlen;
    HANDLE hThread;


    // 화면 초기화 쓰레드
    HANDLE h_cThread = CreateThread(NULL, 0, SleepCls,
        NULL, 0, NULL);

    while (1) {

        if (users > 3) {
            cout << "소캣 생성 실패, 사람이 꽉 찼습니다" << endl;
            continue;
        }
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            printf("accept()");
            break;
        }

        // 접속한 클라이언트 정보 출력
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        printf("\r\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\r\n",
            addr, ntohs(clientaddr.sin_port));

        USER client;
        client.client_sock = client_sock;

        for (int i = 0; i < MAXUSER; ++i)
        {
            if (player[i] == NULL)
            {
                client.id = i;
                users++;
                break;
            }
        }

        // 스레드 생성
        hThread = CreateThread(NULL, 0, ProcessClient,
            (LPVOID)&client, 0, NULL);
        if (hThread == NULL) {
            closesocket(client_sock);
        }
        else {
            CloseHandle(hThread);
        }
    }
    // 소켓 닫기
    closesocket(listen_sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}

// 화면 초기화
DWORD WINAPI SleepCls(LPVOID arg)
{
    while (true)
    {
        Sleep(2000);
        //system("cls");
        printf("서버 접속자 수 %d / %d\n", users, MAXUSER);
    }
}

// 클라이언트와 데이터 통신
DWORD WINAPI ProcessClient(LPVOID arg)
{
    int retval;
    USER* usr = (USER*)arg;
    int id = usr->id;

    SOCKET player_sock = usr->client_sock;
    struct sockaddr_in Tclientaddr;
    char addr[INET_ADDRSTRLEN];
    int Taddrlen;

    // 클라이언트 정보 얻기
    Taddrlen = sizeof(Tclientaddr);
    getpeername(player_sock, (struct sockaddr*)&Tclientaddr, &Taddrlen);
    inet_ntop(AF_INET, &Tclientaddr.sin_addr, addr, sizeof(addr));

    char idbuf[sizeof(int)];
    memcpy(idbuf, &id, sizeof(int));
    send(player_sock, idbuf, sizeof(int), 0);

    delete player[id];
    player[id] = new Player({ 0,0,0 });
    monsterManager->AddPlayer(player[id], id);
    waveManager->AddPlayer(player[id], id);

    while (1)
    {
        if (!updateOn) continue;

        bulletManager->SendBuf(player_sock);
        monsterManager->SendBuf(player_sock);
        turretManager->SendBuf(player_sock);
        // ====================================

        bool result = player[id]->PlayerRecv(player_sock);
        if (!result) {
            delete player[id];
            player[id] = NULL;
            monsterManager->DeletePlayer(id);
            waveManager->DeletePlayer(id);
            users--;
            return 0;
        }
        if (player[id]->IsInstalled())
        {
            for (int i = 0; i < MAXUSER; ++i) {
                if (player[i] != NULL)
                    player[i]->TeamInstall_Turret();
            }
            player[id]->InstallDone();
        }
        player[id]->PlayerSend(player_sock, id);

        // ====================================
        SendAllPlayersInfo(player_sock);
        // ====================================

        //Sleep(1000/60);
    }

    // 소켓 닫기
    closesocket(player_sock);
    
    printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
        addr, ntohs(Tclientaddr.sin_port));

    for (size_t i = 0; i < users; i++)
    {
        if (player[i] == nullptr && id != i)
        {
            player[i] = new Player(*player[id]);
            break;
        }
    }

    delete player[id];
    player[id] = nullptr;

    if (users > 0)users--;

    return 0;
} 

struct PlayersInfo
{
    char gameover[sizeof(bool)];
    char playerOn[sizeof(bool) * 3];
    char pos[sizeof(glm::vec3) * MAXUSER];
    char bodylook[sizeof(glm::vec3) * MAXUSER];
    char headlook[sizeof(glm::vec3) * MAXUSER];
    char legRlook[sizeof(glm::vec3) * MAXUSER];
    char legLlook[sizeof(glm::vec3) * MAXUSER];
    char gunlook[sizeof(glm::vec3) * MAXUSER];
    char guntype[sizeof(GunType) * MAXUSER];
    char gunquat[sizeof(glm::quat) * MAXUSER];
};

GLvoid SendAllPlayersInfo(SOCKET& sock)
{
    PlayersInfo playersInfo;
    char buf[sizeof(PlayersInfo)];

    bool isover = IsGameOver();
    memcpy(playersInfo.gameover, &isover, sizeof(bool));

    bool playerOn[MAXUSER];
    for (int i = 0; i < MAXUSER; ++i) {
        if (player[i] != nullptr)
            playerOn[i] = true;
        else
            playerOn[i] = false;
    }
    memcpy(playersInfo.playerOn, &playerOn, sizeof(bool)* MAXUSER);
    glm::vec3 vPos[MAXUSER]; 
    glm::vec3 vBodyLook[MAXUSER];
    glm::vec3 vHeadLook[MAXUSER];
    glm::vec3 vGunLook[MAXUSER]; 
    glm::vec3 vLegLLook[MAXUSER];
    glm::vec3 vLegRLook[MAXUSER];
    GunType gunType[MAXUSER];
    glm::quat gunRotation[MAXUSER];

    for (size_t i = 0; i < MAXUSER; i++)
    {
        if (player[i] == nullptr) continue;
        vPos[i] = player[i]->GetPosition();
        vBodyLook[i] = player[i]->GetBodyLook();
        vHeadLook[i] = player[i]->GetHeadLook();
        vLegLLook[i] = player[i]->GetLegLLook();
        vLegRLook[i] = player[i]->GetLegRLook();
        vGunLook[i] = player[i]->GetGunLook();
        gunRotation[i] = player[i]->GetGunRotation();
        gunType[i] = player[i]->GetGunType();     
    }
    memcpy(playersInfo.pos, vPos, sizeof(glm::vec3) * MAXUSER);
    memcpy(playersInfo.bodylook, vBodyLook, sizeof(glm::vec3) * MAXUSER);
    memcpy(playersInfo.headlook, vHeadLook, sizeof(glm::vec3) * MAXUSER);
    memcpy(playersInfo.legLlook, vLegLLook, sizeof(glm::vec3) * MAXUSER);
    memcpy(playersInfo.legRlook, vLegRLook, sizeof(glm::vec3) * MAXUSER);
    memcpy(playersInfo.gunlook, vGunLook, sizeof(glm::vec3) * MAXUSER);
    memcpy(playersInfo.guntype, gunType, sizeof(GunType) * MAXUSER);
    memcpy(playersInfo.gunquat, &gunRotation, sizeof(glm::quat) * MAXUSER);

    memcpy(buf, &playersInfo, sizeof(PlayersInfo));
    send(sock, buf, sizeof(PlayersInfo), 0);
}