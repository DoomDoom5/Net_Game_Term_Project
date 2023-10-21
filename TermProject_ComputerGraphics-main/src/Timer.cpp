#include "stdafx.h"
#include "Timer.h"
#include "Object.h"
#include "Camera.h"
#include "Light.h"

#define EXIT -1
#define INIT -2
#define READY -3


GLboolean LightRotation_Y(GLint value);

static unordered_map<Timer, GLboolean(*)(GLint)> timerTable = {
	{ Timer::LightRotation_Y, LightRotation_Y },
};

static set<GLboolean(*)(GLint)> timers;
static GLboolean isUpdate = GL_FALSE;



// fps
static GLfloat deltaTime = 0;
static DWORD lastTime = 0;
static DWORD fps_lastTime = 0;

inline GLvoid PrintFPS(const GLfloat& fps)
{
	SetConsoleCursor(0, 0);
	printf("fps : %.3f\n", fps);
}
GLvoid timer::CalculateFPS()
{
	static GLfloat fps = 0;
	static size_t frameCount = 0;
	static DWORD crntTime = 0;

	++frameCount;

	crntTime = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = (crntTime - lastTime) / 1000.0f;

	GLint timeInterval = crntTime - fps_lastTime;

	if (timeInterval > 1000)
	{
		fps = frameCount / (timeInterval / 1000.0f);

		frameCount = 0;

		fps_lastTime = crntTime;
	}

	lastTime = crntTime;
	PrintFPS(fps);
}
GLfloat timer::DeltaTime()
{
	return deltaTime;
}


// init
GLvoid timer::Init()
{
	for (size_t i = 0; i < timerTable.size(); ++i)
	{
		Timer timer = static_cast<Timer>(i);
		InitTimer(timerTable[timer]);
	}

	lastTime = glutGet(GLUT_ELAPSED_TIME);
	fps_lastTime = glutGet(GLUT_ELAPSED_TIME);

	timers.clear();
}
GLvoid timer::InitTimer(const Timer& timer)
{
	timer::InitTimer(timerTable[timer]);
}
GLvoid timer::InitTimer(GLboolean(*timerFunc)(GLint))
{
	timerFunc(INIT);
	
	if (timers.find(timerFunc) != timers.end())
	{
		timers.erase(timerFunc);
	}
}
GLvoid timer::ReadyTimer(const Timer& timer)
{
	timerTable[timer](READY);
}

// update
GLvoid timer::StartUpdate()
{
	if (isUpdate)
	{
		return;
	}

	isUpdate = GL_TRUE;
}
GLvoid timer::StopUpdate()
{
	isUpdate = GL_FALSE;
}


GLvoid timer::Update()
{
	if (isUpdate == GL_FALSE)
	{
		return;
	}

	vector<GLboolean(*)(GLint)> exitTimers;
	exitTimers.reserve(timers.size());

	for (GLboolean(*timerFunc)(GLint) : timers)
	{
		if (timerFunc(0) == GL_FALSE)
		{
			exitTimers.emplace_back(timerFunc);
		}
	}
	for (GLboolean(*timerFunc)(GLint) : exitTimers)
	{
		timerFunc(EXIT);
		timers.erase(timerFunc);
	}
}



// control
GLvoid timer::ToggleTimer(const Timer& timer)
{
	timer::ToggleTimer(timerTable[timer]);
}
GLvoid timer::EnableTimer(const Timer& timer)
{
	timer::EnableTimer(timerTable[timer]);
}
GLvoid timer::DisableTimer(const Timer& timer)
{
	timer::DisableTimer(timerTable[timer]);
}

GLvoid timer::ToggleTimer(GLboolean(*timerFunc)(GLint))
{
	if (timers.find(timerFunc) != timers.end())
	{
		timerFunc(EXIT);
		timers.erase(timerFunc);
	}
	else
	{
		timers.insert(timerFunc);
	}
}
GLvoid timer::EnableTimer(GLboolean(*timerFunc)(GLint))
{
	if (timers.find(timerFunc) == timers.end())
	{
		timers.insert(timerFunc);
	}
}
GLvoid timer::DisableTimer(GLboolean(*timerFunc)(GLint))
{
	if (timers.find(timerFunc) != timers.end())
	{
		timerFunc(EXIT);
		timers.erase(timerFunc);
	}
}



// timers
GLboolean LightRotation_Y(GLint value)
{
	static GLint dir = 0;
	switch (value)
	{
	case 0:
		break;
	case INIT:
		dir = LEFT;
		return GL_FALSE;
	case EXIT:
		dir *= -1;
		return GL_FALSE;
	default:
		break;
	}

	extern Light* light;
	light->RotatePosition({ 0, 0, 0 }, Vector3::Up(), 100 * timer::DeltaTime() * dir);

	return GL_TRUE;
}



chrono::steady_clock::time_point start;
GLvoid timer::StartRecord()
{
	start = chrono::high_resolution_clock::now();
}
GLvoid timer::PrintDuration()
{
	auto duration = chrono::high_resolution_clock::now() - start;
	cout << "Duration : " << chrono::duration_cast<chrono::milliseconds>(duration).count() << "ms" << endl;
}