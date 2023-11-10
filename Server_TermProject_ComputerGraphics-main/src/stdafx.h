#pragma once
#define _CRT_SECURE_NO_WARNINGS


#include <cmath>
#include <random>
#include <queue>
#include <vector>
#include <string>
#include <future>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <limits>
#include <algorithm>
#include <chrono>

#include <mutex>
#include <thread>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

#include <myGL/glew.h>
#include <myGL/freeglut.h>
#include <myGL/freeglut_ext.h>
#include <myGL/glm/glm.hpp>
#include <myGL/glm/ext.hpp>
#include <myGL/glm/gtc/matrix_transform.hpp>

#include <fmod.hpp>

// colors
#define BLACK RGB(0, 0, 0)
#define WHITE RGB(255, 255, 255)
#define GRAY RGB(100, 100, 100)
#define RED RGB(255, 0, 0)
#define GREEN RGB(0, 255, 0)
#define AQUA RGB(0, 255, 255)
#define MINT RGB(152, 255, 152)
#define DARK_GREEN RGB(0,100,0)
#define FOREST_GREEN RGB(34,139,34)
#define BLUE RGB(0, 0, 255)
#define NAVY RGB(0,0,128)
#define YELLOW RGB(255, 255, 0)
#define ORANGE RGB(255,165,0)
#define GOLD RGB(255,215,0)
#define DARK_GOLDEN RGB(184,134,11)
#define KHAKI RGB(240,230,140)
#define OLIVE RGB(128,128,0)
#define CYAN RGB(0, 255, 255)
#define DARK_CYAN RGB(0,139,139)
#define PINK RGB(255, 0, 255)
#define BROWN RGB(165, 42, 42)
#define SADDLE_BROWN RGB(139,69,19)
#define SANDY_BROWN RGB(244,164,96)
#define IVORY RGB(255,255,240)
#define LAVENDER RGB(230,230,250)
#define CRIMSON RGB(220,20,60)
#define WOOD RGB(93,41,6)

// functions
#define DEGREE_TO_RADIAN(degree) (GLfloat)(((PI/180) * (degree)))
#define RADIAN_TO_DEGREE(radian) (GLfloat)(((180/PI) * (radian)))

// values
#define LEFT -1
#define RIGHT 1
#define UP 1
#define DOWN -1
#define FRONT 1
#define BACK -1
#define PI 3.14159265f
#define GRAVITY 9.8f

// key values for window
#define KEY_CTRL 0x11
#define KEY_SPACE 0x20
#define KEY_A 0x41
#define KEY_B 0x42
#define KEY_C 0x43
#define KEY_D 0x44
#define KEY_E 0x45
#define KEY_F 0x46
#define KEY_G 0x47
#define KEY_H 0x48
#define KEY_I 0x49
#define KEY_J 0x4A
#define KEY_K 0x4B
#define KEY_L 0x4C
#define KEY_M 0x4D
#define KEY_N 0x4E
#define KEY_O 0x4F
#define KEY_P 0x50
#define KEY_Q 0x51
#define KEY_R 0x52
#define KEY_S 0x53
#define KEY_T 0x54
#define KEY_U 0x55
#define KEY_V 0x56
#define KEY_W 0x57
#define KEY_X 0x58
#define KEY_Y 0x59
#define KEY_Z 0x5A
#define KEY_UP VK_UP
#define KEY_DOWN VK_DOWN
#define KEY_LEFT VK_LEFT
#define KEY_RIGHT VK_RIGHT
#define KEY_SPACEBAR 32
#define KEY_ESCAPE 27

// control
#define IS_KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000 ? 1 : 0))
#define IS_KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000 ? 0 : 1))

// window
#define DEFAULT_SCREEN_POS_X 0
#define DEFAULT_SCREEN_POS_Y 0
#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 1024


using namespace std;

enum class Event { None, KeyDown, KeyUp };
enum class Shader { Color = 0, Light, Texture, Back, None, _count };
enum class Dir { None, Left, Right, Up, Down, Front, Back };
enum class CameraMode { Free, FirstPerson, ThirdPerson, Light };
enum class CollisionType { None, Circle, Rect, };

const GLuint NUM_CORE = thread::hardware_concurrency();
//const GLuint NUM_CORE = 4;
constexpr GLint NUM_OF_SHADER = static_cast<GLint>(Shader::_count) - 1;	// exclusive Shader::None



static unordered_map<Dir, Dir> dir_opposite = {
	{Dir::Left, Dir::Right},
	{Dir::Right, Dir::Left},
	{Dir::Up, Dir::Down},
	{Dir::Down, Dir::Up},
	{Dir::Front, Dir::Back},
	{Dir::Back, Dir::Front},
};

constexpr GLfloat FLOAT_MAX = numeric_limits<GLfloat>::max();
constexpr GLfloat FLOAT_MIN = numeric_limits<GLfloat>::min();

class CompareSet {
public:
	// for use set to glm::vec2
	bool operator() (const glm::vec2& lhs, const glm::vec2& rhs) const
	{
		if (lhs.x < rhs.x) // sort by x
		{
			return GL_TRUE;
		}
		return GL_FALSE;
	}
};


GLfloat GetRandColor();
GLvoid MultiplyVector(const glm::mat4& transform, glm::vec3& vector);
GLvoid Rotate(glm::vec3& vector, const GLfloat& theta, const glm::vec3& axis);


class Vector2 : public glm::vec2 {
public:
	Vector2()
	{
		x = 0;
		y = 0;
	}
	Vector2(const GLfloat& x, const GLfloat& y)
	{
		this->x = x;
		this->y = y;
	}
	Vector2(const GLint& x, const GLint& y)
	{
		this->x = static_cast<GLfloat>(x);
		this->y = static_cast<GLfloat>(y);
	}

	inline constexpr operator glm::vec2() const
	{
		return glm::vec2(x, y);
	}

	inline Vector2 operator+(const Vector2& rhs) const
	{
		return { x + rhs.x, y + rhs.y };
	}
	inline Vector2 operator-(const Vector2& rhs) const
	{
		return { x - rhs.x, y - rhs.y };
	}
	inline Vector2 operator*(const GLfloat& rhs) const
	{
		return { x * rhs, y * rhs };
	}
	inline Vector2 operator/(const GLfloat& rhs) const
	{
		return { x / rhs, y / rhs };
	}
	inline constexpr const GLboolean& operator==(const Vector2& rhs) const
	{
		return (x == rhs.x && y == rhs.y) ? GL_TRUE : GL_FALSE;
	}
	inline constexpr const GLboolean& operator!=(const Vector2& rhs) const
	{
		return (x != rhs.x || y != rhs.y) ? GL_TRUE : GL_FALSE;
	}

	inline Vector2 Normalized() const
	{
		return *this / GetNorm(*this);
	}
	inline GLfloat Norm() const
	{
		return sqrtf(x * x + y * y);
	}
	static inline GLfloat GetNorm(Vector2 v)
	{
		return sqrtf(v.x * v.x + v.y * v.y);
	}
	static inline constexpr GLfloat Dot(const Vector2& lhs, const Vector2& rhs)
	{
		return (lhs.x * rhs.x) + (lhs.y * rhs.y);
	}
	static inline constexpr GLfloat GetZAxis(const Vector2& lhs, const Vector2& rhs)
	{
		return (lhs.x * rhs.y) - (rhs.x - lhs.y);
	}
	static inline GLfloat GetTheta(const Vector2& lhs, const Vector2& rhs)
	{
		GLfloat dot = Vector2::Dot(lhs, rhs);
		return acos(dot / (Vector2::GetNorm(lhs) * Vector2::GetNorm(rhs)));
	}

	static inline Vector2 Up()
	{
		return { 0, -1 };
	}
	static inline Vector2 Down()
	{
		return { 0, 1 };
	}
	static inline Vector2 Left()
	{
		return { -1, 0 };
	}
	static inline Vector2 Right()
	{
		return { 1, 0 };
	}
	static inline Vector2 Zero()
	{
		return { 0, 0 };
	}
	static inline Vector2 GetDest(const Vector2& posCenter, const Vector2& vector, const GLfloat& speed = 1)
	{
		return posCenter + (vector * speed);
	}
	static Vector2 Lerp(const Vector2& src, const Vector2& dst, const GLfloat& alpha)
	{
		Vector2 transform;
		transform.x = (src.x * (1 - alpha)) + (dst.x * alpha);
		transform.y = (src.y * (1 - alpha)) + (dst.y * alpha);
		return transform;
	}
	// Linear transform to find the orthogonal vector of the edge
	static Vector2 Get_Normalized_Proj_Axis(const Vector2& crntPoint, const Vector2& nextPoint)
	{
		const float axisX = -(nextPoint.y - crntPoint.y);
		const float axisY = nextPoint.x - crntPoint.x;
		const float magnitude = hypot(axisX, axisY);

		Vector2 axisNormalized;
		axisNormalized.x = axisX / magnitude;
		axisNormalized.y = axisY / magnitude;

		return axisNormalized;
	}

	static inline GLvoid PrintPos(const glm::vec2& v)
	{
		printf("(%.3f, %.3f)\n", v.x, v.y);
	}
};

class Vector3 : public glm::vec3 {

public:
	Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	//Vector3(const Vector2& vector2)
	//{
	//	this->x = vector2.x;
	//	this->y = vector2.y;
	//	this->z = 0;
	//}
	Vector3(const glm::vec3& vector3)
	{
		this->x = vector3.x;
		this->y = vector3.y;
		this->z = vector3.z;
	}
	Vector3(const GLfloat& x, const GLfloat& y, const GLfloat& z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	//inline constexpr operator Vector2 () const
	//{
	//	return { x, y };
	//}
	inline constexpr operator glm::vec3() const
	{
		return { x, y, z };
	}
	inline constexpr GLboolean operator==(const Vector3& rhs) const
	{
		return (x == rhs.x && y == rhs.y && z == rhs.z) ? GL_TRUE : GL_FALSE;
	}
	inline constexpr GLboolean operator!=(const Vector3& rhs) const
	{
		return (x != rhs.x || y != rhs.y || z != rhs.z) ? GL_TRUE : GL_FALSE;
	}
	inline Vector3& operator+=(const Vector3& rhs)
	{
		this->x += rhs.x; this->y += rhs.y; this->z += rhs.z;
		return *this;
	}
	inline Vector3 operator-=(const Vector3& rhs)
	{
		this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z;
		return *this;
	}
	inline Vector3 operator*=(const GLfloat& value)
	{
		this->x *= value; this->y *= value; this->z *= value;
		return *this;
	}
	inline Vector3 operator/=(const GLfloat& value)
	{
		this->x /= value; this->y /= value; this->z /= value;
		return *this;
	}

	inline Vector3 Normalized() const
	{
		return *this / GetNorm(*this);
	}
	inline GLfloat Norm() const
	{
		return sqrtf(x * x + y * y + z * z);
	}
	static inline GLfloat GetNorm(const Vector3& v)
	{
		return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	}
	static inline GLfloat Dot(const Vector3& v, const Vector3& w)
	{
		return (v.x * w.x) + (v.y * w.y) + (w.z * w.z);
	}
	static inline Vector3 Cross(const Vector3& v, const Vector3& w)
	{
		GLfloat x = (v.y * w.z) - (w.y * v.z);
		GLfloat y = (v.z * w.x) - (w.z * v.x);
		GLfloat z = (v.x * w.y) - (w.x * v.y);

		return { x, y, z };
	}
	static inline constexpr GLfloat GetZAxis(const Vector3& lhs, const Vector3& rhs)
	{
		return (lhs.x * rhs.y) - (rhs.x - lhs.y);
	}
	static inline GLfloat GetTheta(const Vector3& lhs, const Vector3& rhs)
	{
		GLfloat dot = Vector3::Dot(lhs, rhs);
		return acos(dot / (Vector3::GetNorm(lhs) * Vector3::GetNorm(rhs)));
	}

	static inline Vector3 Up()
	{
		return { 0, 1, 0 };
	}
	static inline Vector3 Down()
	{
		return { 0, -1, 0 };
	}
	static inline Vector3 Left()
	{
		return { 1, 0, 0 };
	}
	static inline Vector3 Right()
	{
		return { -1, 0, 0 };
	}
	static inline Vector3 Front()
	{
		return { 0, 0, 1 };
	}
	static inline Vector3 Back()
	{
		return { 0, 0, -1 };
	}
	static inline Vector3 Zero()
	{
		return { 0, 0, 0 };
	}

	//static Vector3 RotateAxis(const Vector3& vector, const GLfloat& theta, const Vector3 axis)
	//{
	//	glm::mat4 transform = glm::mat4(1.0f);
	//	Vector3 result = vector;

	//	Vector3 v = axis.Normalized();
	//	v.x = 0;

	//	// xz 평면으로 x축 회전
	//	GLfloat cos_x = Vector3::Dot(v, Vector3::Look());
	//	GLfloat sin_x = Vector3::Cross(v, Vector3::Look()).x;
	//	glm::mat4 transform_x = {
	//		1,		0,		0, 0,
	//		0,	cos_x, -sin_x, 0,
	//		0,	sin_x,	cos_x, 0,
	//		0,		0,		0, 1
	//	};
	//	v = MultiplyVector(transform_x, v);

	//	// z 축으로 y축 회전
	//	GLfloat cos_y = Vector3::Dot(v, Vector3::Look());
	//	GLfloat sin_y = Vector3::Cross(v, Vector3::Look()).y;
	//	glm::mat4 transform_y = {
	//		cos_y,	0,	sin_y,	0,
	//		0,		1,	0,		0,
	//		-sin_y,	0,	cos_x,	0,
	//		0,		0,		0,	1
	//	};
	//	v = MultiplyVector(transform_y, v);

	//	// z축 회전
	//	v = Vector3::RotateOrigin(v, theta, Vector3::Look());
	//	glm::mat4 transform_z = glm::mat4(1.0f);
	//	transform_z = glm::rotate(transform_z, theta, Vector3::Look());

	//	// 역회전 - y
	//	glm::mat4 r_transform_y = {
	//		-cos_y,	0,	-sin_y,	0,
	//		0,		1,	0,		0,
	//		sin_y,	0,	-cos_x,	0,
	//		0,		0,		0,	1
	//	};
	//	v = MultiplyVector(r_transform_y, v);

	//	// 역회전 - x
	//	glm::mat4 r_transform_x = {
	//		1,		0,		0, 0,
	//		0,	-cos_x, sin_x, 0,
	//		0,	-sin_x,	-cos_x, 0,
	//		0,		0,		0, 1
	//	};
	//	v = MultiplyVector(r_transform_x, v);

	//	transform = r_transform_x * r_transform_y * transform_z * transform_y * transform_x;
	//	result = MultiplyVector(transform, vector);


	//	return result;
	//}

	static Vector3 RotateOrigin(const Vector3& vector, const GLfloat& theta, const Vector3& axis)
	{
		Vector3 result = vector;
		if (axis == Vector3::Right())
		{
			result.y = (vector.y * cos(theta)) - (vector.z * sin(theta));
			result.z = (vector.y * sin(theta)) + (vector.z * cos(theta));
		}
		else if (axis == Vector3::Up())
		{
			result.x = (vector.z * sin(theta)) + (vector.x * cos(theta));
			result.z = (vector.z * cos(theta)) - (vector.x * sin(theta));
		}
		else if (axis == Vector3::Front())
		{
			result.x = (vector.x * cos(theta)) - (vector.y * sin(theta));
			result.y = (vector.x * sin(theta)) + (vector.y * cos(theta));
		}
		else
		{
			assert(0);
		}
		return result;
	}

	static inline Vector3 Getdst(Vector3 posCenter, Vector3 vector, const GLfloat& speed = 1)
	{
		return posCenter + (vector * speed);
	}
	static Vector3 Lerp(const Vector3& src, const Vector3& dst, const GLfloat& alpha)
	{
		Vector3 transform;
		transform.x = (src.x * (1 - alpha)) + (dst.x * alpha);
		transform.y = (src.y * (1 - alpha)) + (dst.y * alpha);
		transform.z = (src.z * (1 - alpha)) + (dst.z * alpha);
		return transform;
	}
	static GLfloat GetLength(const Vector3& lhs, const Vector3& rhs)
	{
		Vector3 t = lhs - rhs;
		return(sqrtf(t.x * t.x + t.y * t.y + t.z * t.z));
	}
	static inline GLvoid PrintPos(const Vector3& v)
	{
		printf("(%.3f, %.3f, %.3f)\n", v.x, v.y, v.z);
	}
};


typedef struct MyColor {
	GLfloat r = 1.0f;
	GLfloat g = 1.0f;
	GLfloat b = 1.0f;

	inline constexpr operator glm::vec3() const
	{
		return { r, g, b };
	}
	inline constexpr operator COLORREF() const
	{
		return RGB(r * 255, g * 255, b * 255 );
	}

	MyColor() {}
	MyColor(const COLORREF& color)
	{
		SetColor(color);
	}
	GLvoid Randomize()
	{
		r = GetRandColor();
		g = GetRandColor();
		b = GetRandColor();
	}
	static MyColor GetRandom()
	{
		MyColor result;
		result.Randomize();
		return result;
	}

	GLvoid SetColor(GLint red, GLint green, GLint blue)
	{
		r = (GLfloat)red / 255;
		g = (GLfloat)green / 255;
		b = (GLfloat)blue / 255;
	}
	GLvoid SetColor(COLORREF color)
	{
		GLint red = GetRValue(color);
		GLint green = GetGValue(color);
		GLint blue = GetBValue(color);

		this->SetColor(red, green, blue);
	}
	COLORREF GetColorRef() const
	{
		COLORREF result;
		result = RGB(r * 255, g * 255, b * 255);
	}

	
}MyColor;


class Line {
public:
	glm::vec2 v = { 0,0 };
	glm::vec2 u = { 0,0 };

	Line(const glm::vec2& v, const glm::vec2 u)
	{
		this->v = v;
		this->u = u;
	}
};
class GLrect {
public:
	GLfloat left = 0.0f;
	GLfloat top = 0.0f;
	GLfloat right = 0.0f;
	GLfloat bottom = 0.0f;


	GLrect() {};
	GLrect(const GLfloat& left, const GLfloat& top, const GLfloat& right, const GLfloat& bottom)
	{
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
	}
	GLrect(const glm::vec2& center, const GLfloat& width, const GLfloat& height)
	{
		this->left = center.x - width/2;
		this->top = center.y - height/2;
		this->right = center.x + width/2;
		this->bottom = center.y + height/2;
	}
	glm::vec2 GetCenter() const
	{
		GLfloat x = left + GetWidth() / 2;
		GLfloat y = top + GetHeight() / 2;
		return { x,y };
	}
	GLfloat GetRadius() const
	{
		glm::vec2 center = GetCenter();
		GLfloat v = glm::length(center - glm::vec2(left, top));
		GLfloat u = glm::length(center - glm::vec2(right, bottom));

		return (v > u) ? v : u;
	}
	inline constexpr GLfloat GetWidth() const { return (right - left); }
	inline constexpr GLfloat GetHeight() const { return (bottom - top); }
};

typedef struct GLpoint {
	GLint x = 0;
	GLint y = 0;

	GLvoid Print() const
	{
		printf("(%d, %d)\n", x, y);
	}

	inline GLpoint operator-(const GLpoint& rhs) const
	{
		return { x - rhs.x, y - rhs.y };
	}
	inline GLpoint operator+(const GLpoint& rhs) const
	{
		return { x + rhs.x, y + rhs.y };
	}
	inline GLpoint operator/(const GLpoint& rhs) const
	{
		return { x / rhs.x, y / rhs.y };
	}
	inline GLpoint operator*(const GLpoint& rhs) const
	{
		return { x * rhs.x, y * rhs.y };
	}
	inline constexpr const GLboolean operator==(const GLpoint& rhs) const
	{
		return (x == rhs.x && y == rhs.y) ? GL_TRUE : GL_FALSE;
	}
}GLpoint;



GLvoid Reshape(GLint w, GLint h);
GLvoid RePosition(GLint x, GLint y);
GLvoid ChangePosToGL(GLfloat& x, GLfloat& y);
Vector2 GetPosToGL(const GLint& x, const GLint& y);
COLORREF GetRandRGB();

GLvoid CheckOutOfIndex(const size_t& index, const size_t& size);
GLvoid CheckOutOfIndex(const GLint& index, const GLint& min, const GLint& max);
GLboolean IsOutOfIndex(const size_t& index, const size_t& size);

GLvoid ToggleDepthTest();
GLvoid SetDepthTest(const GLboolean& isDepthTest);

GLfloat GetTheta(const glm::vec2& lhs, const glm::vec2& rhs);
GLvoid GetYawPitch(const glm::vec3& target, GLfloat& yaw, GLfloat& pitch);

template<typename T>
inline GLvoid CopyVector(vector<T>& dst, const vector<T>& src)
{
	dst.resize(src.size());
	std::copy(src.begin(), src.end(), dst.begin());
}



inline Dir GetOpposite(const Dir& dir)
{
	assert(dir_opposite.find(dir) != dir_opposite.end());
	return dir_opposite[dir];
}

template<typename T>
inline T GetRandom(const GLint& size, const T& begin = 0)
{
	return (rand() % size) + begin;
}

template<typename T>
inline constexpr GLfloat GetSign(T num)
{
	return num / fabs(num);
}

inline constexpr GLboolean IsInfinite(const GLfloat& n)
{
	return ((n + 1) != n) && (n == n);
}
inline constexpr glm::vec2 ConvertVec2(const glm::vec3& position)
{
	return glm::vec2(position.x, position.z);
}
inline constexpr glm::vec3 ConvertVec3(const glm::vec2& position)
{
	return glm::vec3(position.x, 0, position.y);
}
inline constexpr glm::vec2 ConvertVec2_YZ(const glm::vec3& position)
{
	return glm::vec2(position.y, position.z);
}

GLvoid RotatePosition(glm::vec3& position, const glm::vec3& pivot, const glm::vec3& axis, const GLfloat& degree);



/* Check Collision 2D Line-Circle */
GLboolean CheckCollision(const glm::vec2& v, const glm::vec2& u, const glm::vec2& center, const  GLfloat& radius);

/* Check Collision 2D Line-Line */
GLboolean CheckCollision(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& u1, const glm::vec2& u2);
/* Must be call when collisioned */
glm::vec2 GetLineIntersection(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& u1, const glm::vec2& u2);

/* Check Collision 2D Circle-Circle */
GLboolean CheckCollision(const glm::vec2& v, const glm::vec2& u, const GLfloat& vRadius, const GLfloat& uRadius);

/* Check Collision 2D Rect-Circle */
GLboolean CheckCollision(const GLrect& rect, const glm::vec2& v, const GLfloat& vRadius);

/* Check Collision 3D Cylinder-Circle */
GLboolean CheckCollision(const glm::vec3& vCylinderPos, const glm::vec3& uPoint, const GLfloat& vRadius, const GLfloat& uRadius, const GLfloat& vHeight);

void SetConsoleCursor(short x, short y);


GLboolean FindEmptyCoreID(mutex& m, unordered_set<GLuint>& emptyCore, GLuint& id);

GLvoid GameOver();
GLboolean IsGameOver();