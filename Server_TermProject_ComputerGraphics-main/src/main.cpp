#include "stdafx.h"
#include "Shader.h"
#include "Object.h"
#include "Player.h"
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

const Camera* crntCamera = nullptr;
Camera* cameraMain = nullptr;
Camera* cameraFree = nullptr;
Camera* cameraTop = nullptr;
CameraMode cameraMode = CameraMode::Free;

GLvoid Init();
GLvoid InitMeshes();
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

// extern

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
	//system("cls");
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

	//********** [ Coordinate system lines ] **********//
	//constexpr GLfloat lineLength = (20.0f / 2.0f);	// radius = 10
	//LineObject* line = nullptr;
	//Vector3 vectorLine_1, vectorLine_2;

	//vectorLine_1 = { -lineLength, 0.0f, 0.0f };
	//vectorLine_2 = { lineLength, 0.0f, 0.0f };
	//line = new LineObject(vectorLine_1, vectorLine_2);
	//line->SetColor(RED);
	//line->MoveGlobal({ lineLength, 0, 0 }, GL_FALSE);
	//AddObject(Shader::Color, line);

	//vectorLine_1 = { 0.0f, -lineLength, 0.0f };
	//vectorLine_2 = { 0.0f, lineLength, 0.0f };
	//line = new LineObject(vectorLine_1, vectorLine_2);
	//line->SetColor(GREEN);
	//line->MoveGlobal({ 0, lineLength, 0 }, GL_FALSE);
	//AddObject(Shader::Color, line);

	//vectorLine_1 = { 0.0f, 0.0f, -lineLength };
	//vectorLine_2 = { 0.0f, 0.0f, lineLength };
	//line = new LineObject(vectorLine_1, vectorLine_2);
	//line->SetColor(BLUE);
	//line->MoveGlobal({ 0, 0, lineLength }, GL_FALSE);
	//AddObject(Shader::Color, line);
	//**************************************************//
	
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
	player = new Player({ 0,0,0 }, &cameraMode);
	uiManager->SetPlayer(player);
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

	if (player != nullptr)
	{
		player->Draw(cameraMode);
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


















///// [ HANDLE EVENTS ] /////
GLvoid Update()
{
	if (IsGameOver() == GL_TRUE)
	{
		glutPostRedisplay();
		return;
	}

	timer::CalculateFPS();
	timer::Update();

	if (player != nullptr)
	{
		player->Update();
	}

	bulletManager->Update();
	monsterManager->Update();
	buildingManager->Update();
	turretManager->Update();
	waveManager->Update();

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

	if (player != nullptr)
	{
		player->ProcessMouse(button, state, x, y);
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
	else if(player != nullptr)
	{
		player->Rotate(dy, dx, 0.0f);
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

	if (player != nullptr)
	{
		player->ProcessKeyDown(key);
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

	if (player != nullptr)
	{
		player->ProcessKeyUp(key);
	}
}
GLvoid ProcessSpecialKeyDown(GLint key, GLint x, GLint y)
{
	if (IsGameOver() == GL_TRUE)
	{
		return;
	}

	// WARNING : (GLUT_KEY_LEFT == 'd') -> 100 //
	//switch (key)
	//{
	//case GLUT_KEY_HOME:
	//	cameraFree->Look({ 0,0,0 });
	//	break;
	//case GLUT_KEY_F1:
	//	isWireFrame = !isWireFrame;
	//	break;
	//}

	if (player != nullptr)
	{
		if (key == GLUT_KEY_LEFT)
		{
			return;
		}

		player->ProcessKeyDown(key);
	}
}
GLvoid ProcessSpecialKeyUp(GLint key, GLint x, GLint y)
{
	if (IsGameOver() == GL_TRUE)
	{
		return;
	}

	if (player != nullptr)
	{
		if (key == GLUT_KEY_LEFT)
		{
			return;
		}
		player->ProcessKeyUp(key);
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
	case CameraMode::Light:
		cameraMain = cameraFree;
		cameraMain->SetPosition(light->GetPviotedPosition());
		cameraMain->SetLook(light->GetLook());
		break;
	}

	cameraMode = mode;
}