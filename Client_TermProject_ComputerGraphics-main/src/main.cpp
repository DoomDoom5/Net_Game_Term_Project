#include "stdafx.h"
#include "Shader.h"
#include "Object.h"
#include "player.h"
#include "Model.h"
#include "Timer.h"
#include "Camera.h"
#include "Transform.h"
#include "Map.h"
#include "Light.h"
#include "Bullet.h"
#include "Monster.h"
#include "Building.h"
#include "Turret.h"
#include "Sound.h"
#include "Wave.h"
#include "UI.h"
#include "Common.h"

char* SERVERIP = (char*)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE    512

const Camera* crntCamera = nullptr;
Camera* cameraMain = nullptr;
Camera* cameraFree = nullptr;
Camera* cameraTop = nullptr;
CameraMode cameraMode = CameraMode::Free;

GLvoid Init();
GLvoid InitMeshes();
GLvoid InitPlayer();
GLvoid DrawScene();

GLvoid Update();
GLvoid Mouse(GLint button, GLint state, GLint x, GLint y);
GLvoid MouseMotion(GLint x, GLint y);
GLvoid MousePassiveMotion(GLint x, GLint y);
GLvoid ProcessKeyDown(unsigned char key, GLint x, GLint y);
GLvoid ProcessKeyUp(unsigned char key, GLint x, GLint y);
GLvoid ProcessSpecialKeyDown(GLint key, GLint x, GLint y);
GLvoid ProcessSpecialKeyUp(GLint key, GLint x, GLint y);

GLvoid ToggleDepthTest();
GLvoid SetCameraMode(const CameraMode& cameraMode);

GLvoid Initsock(SOCKET& sock);
// values
GLint screenPosX = DEFAULT_SCREEN_POS_X;
GLint screenPosY = DEFAULT_SCREEN_POS_Y;
GLint screenWidth = DEFAULT_SCREEN_WIDTH;
GLint screenHeight = DEFAULT_SCREEN_HEIGHT;

// world
glm::vec3 worldPosition(0.0f, 0.0f, 0.0f);
glm::vec3 worldRotation(0.0f, 0.0f, 0.0f);

// light
Light* light = nullptr;

// managers
BulletManager* bulletManager = nullptr;
MonsterManager* monsterManager = nullptr;
BuildingManager* buildingManager = nullptr;
TurretManager* turretManager = nullptr;
SoundManager* soundManager = nullptr;
WaveManager* waveManager = nullptr;
UIManager* uiManager = nullptr;

// objects
Map* crntMap = nullptr;

GLvoid UpdateplayersPos(SOCKET& sock);
Player* player[3] = { nullptr ,nullptr, nullptr};
int retval = 0;
int users = 0;
int myid = 0;

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

// extern
GLint main(GLint argc, GLchar** argv)
{
	srand((unsigned int)time(NULL));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
//	glutInitWindowPosition(DEFAULT_SCREEN_POS_X, DEFAULT_SCREEN_POS_Y);
	//glutInitWindowSize(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);

	// 테스트용 임시 좌표
	glutInitWindowPosition(1000, DEFAULT_SCREEN_POS_Y);
	glutInitWindowSize(DEFAULT_SCREEN_WIDTH/2, DEFAULT_SCREEN_HEIGHT/2);

	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glShadeModel(GL_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutCreateWindow("TestProject");

	glewExperimental = GL_TRUE;

	Init();

	glutIdleFunc(Update);	
	glutDisplayFunc(DrawScene);
	glutReshapeFunc(Reshape);
	glutSetCursor(GLUT_CURSOR_NONE);
	
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MousePassiveMotion);

	glutPositionFunc(RePosition);
	glutKeyboardFunc(ProcessKeyDown);
	glutKeyboardUpFunc(ProcessKeyUp);
	glutSpecialFunc(ProcessSpecialKeyDown);
	glutSpecialUpFunc(ProcessSpecialKeyUp);
	timer::StartUpdate();

	glutMainLoop();
}

// sock
SOCKET sock = NULL;

///// INIT /////
MyColor backColor;

GLvoid Init()
{
	glewInit();
	shd::Init();
	InitLight();
	InitMeshes();
	timer::Init();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	backColor.SetColor(CYAN);

	//********** [ Camera ] **********//
	cameraFree = new Camera({ 0, 200.0f, 100.0f });
	cameraFree->Look({ 0,0,0 });
	cameraFree->SetFovY(110);

	cameraTop = new Camera();
	cameraTop->RotateLocal(89.9f, 0.0f, 0.0f);
	cameraTop->SetPerpective(GL_FALSE);

	cameraMain = cameraFree;
	crntCamera = cameraMain;
	//********************************//

	mouseCenter = { screenWidth / 2 + screenPosX, screenHeight / 2 + screenPosY };

	waveManager->Start();
	soundManager->PlayBGMSound(BGMSound::Normal, 0.2f, GL_TRUE);

	//************ [Server]************
	cout << "접속 IP를 입력해 주세요, 그냥 00는 127.0.0.1로 연결됩니다. : ";
	char ip[22];
	cin >> ip;
	if (strlen(ip) > 3) SERVERIP = (char*)ip;

	if (sock == NULL)Initsock(sock);
	InitPlayer();
	system("cls");
}

GLvoid InitMeshes()
{
	InitModels();
	InitObject();
	bulletManager = new BulletManager();
	monsterManager = new MonsterManager();
	buildingManager = new BuildingManager();
	turretManager = new TurretManager();
	soundManager = new SoundManager();
	waveManager = new WaveManager();
	uiManager = new UIManager();

	buildingManager->Create(BuildingType::Core, { 0, 0, 550 });

	// test object
	const Model* cubeMapModel = GetTextureModel(Textures::CubeMap);
	cubeMap = new ModelObject(cubeMapModel, Shader::Texture);
	cubeMap->SetTexture(Textures::CubeMap);
	cubeMap->Scale(150);
	cubeMap->SetPosY(-cubeMap->GetHeight() / 2);

	// light object
	light = new Light();
	light->SetPosition({ 0, 400, 0 });

	crntMap = new Map();
}

GLvoid InitPlayer()
{
	char buf[sizeof(int)];
	retval = recv(sock, buf, sizeof(int), 0);
	memcpy(&myid, buf, sizeof(int));

	for (int i = 0; i < 3; ++i)
	{
		if (i != myid)player[i] = new Player({ 10 * i,0,10 * i });
		else player[i] = new Player({ 0,0,0 }, &cameraMode);
	}
	uiManager->SetPlayer(player[myid]);
	monsterManager->SetPlayer(player[myid]);
	waveManager->SetPlayer(player[myid]);
}

GLvoid Reset()
{
	DeleteObjects();

	delete bulletManager;
	delete monsterManager;
	delete buildingManager;
	delete turretManager;
	delete soundManager;
	delete waveManager;

	bulletManager = nullptr;
	monsterManager = nullptr;
	buildingManager = nullptr;
	turretManager = nullptr;
	soundManager = nullptr;
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
	if (player[myid] != nullptr)
	{
		delete player[myid];
		player[myid] = nullptr;
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
		glViewport(halfWidth + halfWidth/2, halfHeight, halfWidth/2, halfHeight);
		SetDepthTest(GL_FALSE);
		return;
	default:
		assert(0);
	}
}
GLvoid DrawScene()
{
	glClearColor(backColor.r, backColor.g, backColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (isWireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT, GL_FILL);
	}

	SetWindow(0);

	Shader crntShader = Shader::Color;
	shd::Use(crntShader);
	shd::SetShader(crntShader, "viewTransform", xform::GetView(crntCamera));
	shd::SetShader(crntShader, "projTransform", xform::GetProj(crntCamera));
	DrawObjects(crntShader);

	crntShader = Shader::Light;
	shd::Use(crntShader);
	shd::SetShader(crntShader, "viewTransform", xform::GetView(crntCamera));
	shd::SetShader(crntShader, "projTransform", xform::GetProj(crntCamera));
	shd::SetShader(crntShader, "light.pos", light->GetPviotedPosition());
	shd::SetShader(crntShader, "viewPos", crntCamera->GetPviotedPosition());
	DrawObjects(crntShader);
	bulletManager->Draw();

	//light->Draw();

	crntShader = Shader::Texture;
	shd::Use(crntShader);
	shd::SetShader(crntShader, "viewTransform", xform::GetView(crntCamera));
	shd::SetShader(crntShader, "projTransform", xform::GetProj(crntCamera));
	shd::SetShader(crntShader, "light.pos", light->GetPviotedPosition());
	shd::SetShader(crntShader, "viewPos", crntCamera->GetPviotedPosition());
	DrawObjects(crntShader);
	turretManager->Draw();
	monsterManager->Draw();
	buildingManager->Draw();

	for (int i = 0; i < users; ++i)
	{
		if (player[i] == nullptr) continue;
		if (myid == i)
			player[i]->Draw(cameraMode);
		else
			player[i]->Draw(CameraMode::Free);
	}

	glCullFace(GL_FRONT);
	cubeMap->Draw();
	glCullFace(GL_BACK);

	DrawBlendObjects();

	shd::Use(Shader::Back);
	uiManager->Draw();

	glBindVertexArray(0);
	glutSwapBuffers();
}

/// Network
GLvoid Initsock(SOCKET& sock)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	 sock = socket(AF_INET, SOCK_STREAM, 0);
//	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	printf("initsocket 함수\n");
}

///// [ HANDLE EVENTS ] /////
GLvoid Update()
{
	// 데이터 수신
	//RecvfromServer();

	if (IsGameOver() == GL_TRUE)
	{
		glutPostRedisplay();

		// 소켓 닫기
		closesocket(sock);
		// 윈속 종료
		WSACleanup();

		return;
	}

	timer::CalculateFPS();
	timer::Update();

	monsterManager->Update(sock);
	for (int i = 0; i < users; ++i) player[i]->Update();
	if (player[myid] != nullptr) player[myid]->PlayerSend(sock);
	if (player[myid] != nullptr) player[myid]->PlayerRecv(sock);
	if (player[myid] != nullptr) UpdateplayersPos(sock);

	//bulletManager->Update(sock);
	//turretManager->Update(sock);
	//waveManager->Update(sock);
	
	constexpr GLfloat cameraMovement = 100.0f;
	GLfloat cameraSpeed = cameraMovement;
	// movement
	if (cameraMain == cameraFree)
	{
		if (IS_KEY_DOWN(KEY_UP))
		{
			cameraMain->MoveZ(cameraSpeed);
			if (cameraMode == CameraMode::Light)
			{
				light->MoveZ(cameraSpeed);
			}
		}
		if (IS_KEY_DOWN(KEY_DOWN))
		{
			cameraMain->MoveZ(-cameraSpeed);
			if (cameraMode == CameraMode::Light)
			{
				light->MoveZ(-cameraSpeed);
			}
		}
		if (IS_KEY_DOWN(KEY_LEFT))
		{
			cameraMain->MoveX(-cameraSpeed);
			if (cameraMode == CameraMode::Light)
			{
				light->MoveX(-cameraSpeed);
			}
		}
		if (IS_KEY_DOWN(KEY_RIGHT))
		{
			cameraMain->MoveX(cameraSpeed);
			if (cameraMode == CameraMode::Light)
			{
				light->MoveX(cameraSpeed);
			}
		}
		if (IS_KEY_DOWN(VK_NEXT))
		{
			cameraMain->MoveGlobal({ 0, -cameraSpeed, 0 });
			if (cameraMode == CameraMode::Light)
			{
				light->MoveGlobal({ 0, -cameraSpeed, 0 });
			}
		}
		if (IS_KEY_DOWN(VK_PRIOR))
		{
			//cameraMain->MoveGlobal({ 0, cameraSpeed, 0 });
			if (cameraMode == CameraMode::Light)
			{
				light->MoveGlobal({ 0, -cameraSpeed, 0 });
			}
		}
	}
	glutPostRedisplay();
}

GLvoid Mouse(GLint button, GLint state, GLint x, GLint y)
{
	if (IsGameOver() == GL_TRUE)
	{
		return;
	}

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			isLeftDown = GL_TRUE;
		}
		else if (state == GLUT_UP)
		{
			isLeftDown = GL_FALSE;
		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			isRightDown = GL_TRUE;
		}
		else if (state == GLUT_UP)
		{
			isRightDown = GL_FALSE;
		}
		break;
	}

	if (player[myid] != nullptr)
	{
		player[myid]->ProcessMouse(button, state, x, y);
	}
}
GLvoid MouseMotion(GLint x, GLint y)
{
	if (IsGameOver() == GL_TRUE)
	{
		return;
	}

	MousePassiveMotion(x, y);
}
GLvoid MousePassiveMotion(GLint x, GLint y)
{
	if (IsGameOver() == GL_TRUE)
	{
		return;
	}

	POINT cursorPos;
	GetCursorPos(&cursorPos);
	crntPos = { cursorPos.x, cursorPos.y };

	GLfloat sensitivity = 10;
	GLfloat dx = (mouseCenter.x - crntPos.x) / sensitivity;
	GLfloat dy = (mouseCenter.y - crntPos.y) / sensitivity;
	
	//d = d * 50.0f;

	if (cameraMain == cameraFree)
	{
		cameraFree->RotateLocal(dy, dx, 0.0f);
		if (cameraMode == CameraMode::Light)
		{
			light->RotateLocal(dy, dx, 0.0f);
		}
	}
	else if(player[myid] != nullptr)
	{
		player[myid]->Rotate(dy, dx, 0.0f);
	}

	SetCursorPos(mouseCenter.x, mouseCenter.y);
}

// interlock with a control key
static unordered_map<unsigned char, unsigned char> CtrlMap = {
	{23, 'w'},
	{19, 's'},
	{1, 'a'},
	{4, 'd'},
};
GLvoid ProcessKeyDown(unsigned char key, GLint x, GLint y)
{
	if (IsGameOver() == GL_TRUE)
	{
		if (key == KEY_ESCAPE)
		{
			glutLeaveMainLoop();
		}

		return;
	}

	if (CtrlMap.find(key) != CtrlMap.end())
	{
		key = CtrlMap[key];
	}

	switch (key)
	{
		// controls
	case '`':
		//Reset();
		break;
	case 'm':
	case 'M':
		light->ToggleLight();
		break;

		// camera
	case 'p':
	case 'P':
		cameraMain->SetPerpective(GL_TRUE);
		break;
	case 'o':
	case 'O':
		cameraMain->SetPerpective(GL_FALSE);
		break;
	case '1':
		SetCameraMode(CameraMode::Free);
		break;
	case '2':
		SetCameraMode(CameraMode::FirstPerson);
		break;
	case '3':
		SetCameraMode(CameraMode::ThirdPerson);
		break;
	case '0':
		//SetCameraMode(CameraMode::Light);
		break;
		// objects

	case KEY_ESCAPE:
		glutLeaveMainLoop();
		break;
	default:
		break;

	}

	if (player[myid] != nullptr)
	{
		player[myid]->ProcessKeyDown(key);
	}
}
GLvoid ProcessKeyUp(unsigned char key, GLint x, GLint y)
{
	if (IsGameOver() == GL_TRUE)
	{
		return;
	}

	if (CtrlMap.find(key) != CtrlMap.end())
	{
		key = CtrlMap[key];
	}

	if (player[myid] != nullptr)
	{
		player[myid]->ProcessKeyUp(key);
	}
}
GLvoid ProcessSpecialKeyDown(GLint key, GLint x, GLint y)
{
	if (IsGameOver() == GL_TRUE)
		return;

	if (player[myid] != nullptr)
	{
		if (key == GLUT_KEY_LEFT)
		{
			return;
		}

		player[myid]->ProcessKeyDown(key);
	}
}
GLvoid ProcessSpecialKeyUp(GLint key, GLint x, GLint y)
{
	if (IsGameOver() == GL_TRUE)
	{
		return;
	}

	if (player[myid] != nullptr)
	{
		if (key == GLUT_KEY_LEFT)
		{
			return;
		}
		player[myid]->ProcessKeyUp(key);
	}
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
		if (player[myid] != nullptr)
		{
			cameraMain = player[myid]->GetFirstPersonCamera();
		}
		break;
	case CameraMode::ThirdPerson:
		if (player[myid] != nullptr)
		{
			cameraMain = player[myid]->GetThirdPersonCamera();
		}
		break;
	case CameraMode::Light:
		cameraMain = cameraFree;
		cameraMain->SetPosition(light->GetPviotedPosition());
		cameraMain->SetLook(light->GetLook());
		break;
	}

	cameraMode = mode;
}

struct PlayersInfo
{
	char gameover[sizeof(bool)];
	char num[sizeof(int)];
	char pos[sizeof(uint32_t) * 3 * MAXUSER];
	char bodylook[sizeof(uint32_t) * 3 * MAXUSER];
	char headlook[sizeof(uint32_t) * 3 * MAXUSER];
	char gunpos[sizeof(uint32_t) * 3 * MAXUSER];
	char gunlook[sizeof(uint32_t) * 3 * MAXUSER];
	char guntype[sizeof(GunType) * MAXUSER];
	char gunquat[sizeof(glm::quat) * MAXUSER];
};

GLvoid UpdateplayersPos(SOCKET& sock)
{
	PlayersInfo playerInfo;
	int retval = 0;
	char buf[sizeof(PlayersInfo)];
	retval = recv(sock, buf, sizeof(PlayersInfo), 0);
	memcpy(&playerInfo, buf, sizeof(PlayersInfo));

	bool isover;
	memcpy(&isover, playerInfo.gameover, sizeof(bool));
	if (isover) GameOver();
	memcpy(&users, playerInfo.num, sizeof(int));
	cout << "myID: " << myid << endl;
	cout << "RecvFromServer: " << endl;

	uint32_t nPos[MAXUSER * 3];
	uint32_t nBodyLook[MAXUSER * 3];
	uint32_t nHeadLook[MAXUSER * 3];
	uint32_t nGunPos[MAXUSER * 3]; 
	uint32_t nGunLook[MAXUSER * 3];
	GunType gunType[MAXUSER];
	glm::quat gunRotation[MAXUSER];

	memcpy(nPos, playerInfo.pos, sizeof(uint32_t) * 3 * users);
	memcpy(nBodyLook, playerInfo.bodylook, sizeof(uint32_t) * 3 * users);
	memcpy(nHeadLook, playerInfo.headlook, sizeof(uint32_t) * 3 * users);
	memcpy(nGunPos, playerInfo.gunpos, sizeof(uint32_t) * 3 * users);
	memcpy(nGunLook, playerInfo.gunlook, sizeof(uint32_t) * 3 * users);
	memcpy(gunType, playerInfo.guntype, sizeof(GunType) * users);
	memcpy(gunRotation, playerInfo.gunquat, sizeof(glm::quat) * users);

	int id = 0;
	for (size_t i = 0; i < users; i++)
	{
		glm::vec3 fPos;
		glm::vec3 fBodyLook;
		glm::vec3 fHeadLook;
		glm::vec3 fGunPos;
		glm::vec3 fGunLook;
		if (player[i] == nullptr) return;
		fPos.x = *reinterpret_cast<float*>(&nPos[3 * i + 0]);
		fPos.y = *reinterpret_cast<float*>(&nPos[3 * i + 1]);
		fPos.z = *reinterpret_cast<float*>(&nPos[3 * i + 2]);
		fBodyLook.x = *reinterpret_cast<float*>(&nBodyLook[3 * i + 0]);
		fBodyLook.y = *reinterpret_cast<float*>(&nBodyLook[3 * i + 1]);
		fBodyLook.z = *reinterpret_cast<float*>(&nBodyLook[3 * i + 2]);
		fHeadLook.x = *reinterpret_cast<float*>(&nHeadLook[3 * i + 0]);
		fHeadLook.y = *reinterpret_cast<float*>(&nHeadLook[3 * i + 1]);
		fHeadLook.z = *reinterpret_cast<float*>(&nHeadLook[3 * i + 2]);
		fGunPos.x = *reinterpret_cast<float*>(&nGunPos[3 * i + 0]);
		fGunPos.y = *reinterpret_cast<float*>(&nGunPos[3 * i + 1]);
		fGunPos.z = *reinterpret_cast<float*>(&nGunPos[3 * i + 2]);
		fGunLook.x = *reinterpret_cast<float*>(&nGunLook[3 * i + 0]);
		fGunLook.y = *reinterpret_cast<float*>(&nGunLook[3 * i + 1]);
		fGunLook.z = *reinterpret_cast<float*>(&nGunLook[3 * i + 2]);

		cout << i << " Pos: ( " << fPos.x << ", " << fPos.y << ", " << fPos.z << " )" << endl;
		cout << i << " BodyLook: ( " << fBodyLook.x << ", " << fBodyLook.y << ", " << fBodyLook.z << " )" << endl;
		cout << i << " HeadLook: ( " << fHeadLook.x << ", " << fHeadLook.y << ", " << fHeadLook.z << " )" << endl;
		cout << i << " GunPos: (" << fGunPos.x << ", " << fGunPos.y << ", " << fGunPos.z << ")" << endl;
		cout << i << " GunLook: (" << fGunLook.x << ", " << fGunLook.y << ", " << fGunLook.z << ")" << endl;

		if (i == myid) continue;

		player[i]->SetGunType(gunType[i]);
		player[i]->SetPosition(fPos);
		player[i]->SetBodyLook(fBodyLook);
		player[i]->SetHeadLook(fHeadLook);
		player[i]->SetGunPos(fGunLook);
		player[i]->SetGunLook(fGunLook);
		player[i]->SetGunRotation(gunRotation[i]);
	}
}