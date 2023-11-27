#include "stdafx.h"
#include "Object.h"
#include "Player.h"
#include "Model.h"
#include "Timer.h"
#include "Camera.h"
#include "Transform.h"
#include "Map.h"
#include "Bullet.h"
#include "Monster.h"
#include "Building.h"
#include "Turret.h"
#include "Wave.h"
#include "Common.h"

#define SERVERPORT 9000
#define BUFSIZE    512

char* SERVERIP = (char*)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE 50

const Camera* crntCamera = nullptr;
Camera* cameraMain = nullptr;
Camera* cameraFree = nullptr;
Camera* cameraTop = nullptr;
CameraMode cameraMode = CameraMode::Free;

GLvoid Init();
GLvoid InitMeshes();
GLvoid DrawScene();

GLvoid Update();

GLvoid ToggleDepthTest();
GLvoid SetCameraMode(const CameraMode& cameraMode);

// network
GLvoid init_Listen_Sock(SOCKET& listen_sock);
GLvoid init_Client_Sock(SOCKET& client_sock, sockaddr_in& clientaddr, int addrlen);


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
Player* player = nullptr;

// modes
GLboolean isPersp = GL_TRUE;
GLboolean isCulling = GL_TRUE;
GLboolean isWireFrame = GL_FALSE;

// mouse
GLpoint mouseCenter = { 0,0 };
GLpoint crntPos = { 0,0 };
GLboolean isLeftDown = GL_FALSE;
GLboolean isRightDown = GL_FALSE;

// temp
ModelObject* cubeMap = nullptr;

// socket
SOCKET listen_sock = NULL;

// 데이터 통신에 사용할 변수
SOCKET client_sock = NULL;
struct sockaddr_in clientaddr;
int addrlen;
// 일단은 1 : 1 플레이

GLint main(GLint argc, GLchar** argv)
{
	srand((unsigned int)time(NULL));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(DEFAULT_SCREEN_POS_X, DEFAULT_SCREEN_POS_Y);
	glutInitWindowSize(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glShadeModel(GL_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutCreateWindow("TestProject");
	glewExperimental = GL_TRUE;

	Init();

	glutIdleFunc(Update);
	glutDisplayFunc(DrawScene);
	glutReshapeFunc(Reshape);

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

	mouseCenter = { screenWidth / 2 + screenPosX, screenHeight / 2 + screenPosY };

    waveManager->Start();

	//************ [Server]************
	if (listen_sock == NULL) init_Listen_Sock(listen_sock);
	while (client_sock == NULL) init_Client_Sock(client_sock, clientaddr, addrlen);

	system("cls");
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

	// test object
	const Model* cubeMapModel = GetTextureModel(Textures::CubeMap);
	cubeMap = new ModelObject(cubeMapModel, Shader::Texture);
	cubeMap->SetTexture(Textures::CubeMap);
	cubeMap->Scale(150);
	cubeMap->SetPosY(-cubeMap->GetHeight() / 2);

	crntMap = new Map();
	player = new Player({ 0,0,0 }, &cameraMode);
	monsterManager->SetPlayer(player);
	waveManager->SetPlayer(player);
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
    if (player != nullptr)
    {
        delete player;
        player = nullptr;
    }
    cameraMode = CameraMode::Free;

    Init();
}


///// Draw /////

GLvoid SetWindow(GLint index)
{
    const GLint halfWidth = screenWidth / 2;
    const GLint halfHeight = screenHeight / 2;
    switch (index)
    {
    case 0:
        crntCamera = cameraMain;
        glViewport(0, 0, screenWidth, screenHeight);
        SetDepthTest(GL_TRUE);
        return;
    case 1:
        crntCamera = cameraTop;
        glViewport(halfWidth + halfWidth / 2, halfHeight, halfWidth / 2, halfHeight);
        SetDepthTest(GL_FALSE);
        return;
    default:
        assert(0);
    }
}

GLvoid DrawScene()
{
    glutSwapBuffers();
}

///// [ HANDLE EVENTS ] /////
GLvoid Update()
{

    system("cls");

	if (IsGameOver() == GL_TRUE)
	{
	//	glutPostRedisplay();
        return;
	}

    timer::CalculateFPS();
    timer::Update();


    if (player != nullptr) player->Update(client_sock);
	bulletManager->Update(client_sock);
	monsterManager->Update(client_sock);
	//buildingManager->Update(client_sock);
	//turretManager->Update(client_sock);
	//waveManager->Update(client_sock);

    glutPostRedisplay();

}

GLvoid SetCameraMode(const CameraMode& mode)
{
    if (IsGameOver() == GL_TRUE)
    {
        return;
    }

    switch (mode)
    {
    case CameraMode::Free:
        cameraMain = cameraFree;
        break;
    case CameraMode::FirstPerson:
        if (player != nullptr)
        {
            cameraMain = player->GetFirstPersonCamera();
        }
        break;
    case CameraMode::ThirdPerson:
        if (player != nullptr)
        {
            cameraMain = player->GetThirdPersonCamera();
        }
        break;
 
    }

    cameraMode = mode;
}

GLvoid init_Listen_Sock(SOCKET& sock)
{
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return;

    // 소켓 생성
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    // bind()
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = ::bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");
    bind(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    // listen()
    retval = listen(sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    cout << "서버 생성 완료" << endl;
    return;
}

GLvoid init_Client_Sock(SOCKET& Client_sock, sockaddr_in& clientaddr, int addrlen)
{
    // accept()
    addrlen = sizeof(clientaddr);
    client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
    if (client_sock == INVALID_SOCKET) {
        err_display("accept()");
        return;
    }

    // 접속한 클라이언트 정보 출력
    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
    printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
        addr, ntohs(clientaddr.sin_port));
}
GLvoid sendVector(SOCKET& sock)
{
    int retval;

    // 데이터 통신에 사용할 변수
    SOCKET client_sock;
    struct sockaddr_in clientaddr;
    int addrlen;
    while (1) {
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(sock, (struct sockaddr*)&clientaddr, &addrlen);
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
        char buffer[BUFSIZE];
        float PlayerX = 0.0f;
        float PlayerY = 0.0f;
        float PlayerZ = 0.0f;
        while (1) {
            // 데이터 송신
            // 데이터 수신
            {
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
                    break;
                }
                printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);
                Sleep(500);
            }
        }
        // 소켓 닫기
        closesocket(client_sock);
        printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
            addr, ntohs(clientaddr.sin_port));
    }
}
