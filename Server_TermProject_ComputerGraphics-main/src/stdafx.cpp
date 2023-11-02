#include "stdafx.h"

extern GLint screenPosX;
extern GLint screenPosY;
extern GLsizei screenWidth;
extern GLsizei screenHeight;





extern GLpoint mouseCenter;
GLvoid Reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	screenWidth = w;
	screenHeight = h;
	
	mouseCenter = { screenWidth / 2 + screenPosX, screenHeight / 2 + screenPosY };
}
GLvoid RePosition(GLint x, GLint y)
{
	screenPosX = x;
	screenPosY = y;

	mouseCenter = { screenWidth / 2 + screenPosX, screenHeight / 2 + screenPosY };
}
GLfloat GetRandColor()
{
	GLint color = GetRandom<GLint>(10000);
	return GLfloat(color * 0.0001);
}
COLORREF GetRandRGB()
{
	GLint r = GetRandom<GLint>(256);
	GLint g = GetRandom<GLint>(256);
	GLint b = GetRandom<GLint>(256);

	return RGB(r, g, b);
}

GLvoid ChangePosToGL(GLfloat& x, GLfloat& y)
{
	x /= screenWidth;
	y /= screenHeight;

	x *= 2;
	y *= 2;

	x -= 1.0f;
	y -= 1.0f;
	y *= -1;
}
Vector2 GetPosToGL(const GLint& x, const GLint& y)
{
	GLfloat toX = (GLfloat)x;
	GLfloat toY = (GLfloat)y;
	ChangePosToGL(toX, toY);
	return { toX, toY };
}
GLvoid CheckOutOfIndex(const size_t& index, const size_t& size)
{
	if (index >= size)
	{
		assert(0);
	}
}
GLboolean IsOutOfIndex(const size_t& index, const size_t& size)
{
	if (index >= size)
	{
		return GL_FALSE;
	}

	return GL_TRUE;
}

GLvoid CheckOutOfIndex(const GLint& index, const GLint& min, const GLint& max)
{
	if (index < min || index >= max)
	{
		assert(0);
	}
}

GLvoid ToggleDepthTest()
{
	if (glIsEnabled(GL_DEPTH_TEST))
	{
		glDisable(GL_DEPTH_TEST);
	}
	else
	{
		glEnable(GL_DEPTH_TEST);
	}
}
GLvoid SetDepthTest(const GLboolean& isDepthTest)
{
	if (isDepthTest)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}

GLvoid MultiplyVector(const glm::mat4& transform, glm::vec3& vector)
{
	glm::vec4 v({ vector , 1.0f });
	v = transform * v;

	vector = { v.x, v.y, v.z };
}

GLvoid Rotate(glm::vec3& vector, const GLfloat& theta, const glm::vec3& axis)
{
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::rotate(transform, theta, axis);
	MultiplyVector(transform, vector);
}

GLvoid RotatePosition(glm::vec3& position, const glm::vec3& pivot, const glm::vec3& axis, const GLfloat& degree)
{
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::rotate(transform, glm::radians(degree), glm::normalize(axis));
	position -= pivot;
	MultiplyVector(transform, position);
	position += pivot;
}

/* https://bukkit.org/threads/how-do-i-get-yaw-and-pitch-from-a-vector.50317/ */
GLvoid GetYawPitch(const glm::vec3& look, GLfloat& yaw, GLfloat& pitch)
{
	GLfloat distance = sqrtf(look.z * look.z + look.x * look.x);

	yaw = RADIAN_TO_DEGREE(atan2(look.y, distance));
	pitch = RADIAN_TO_DEGREE(atan2(look.x, look.z));}

/* https://stackoverflow.com/questions/1073336/circle-line-segment-collision-detection-algorithm */
GLboolean CheckCollision(const glm::vec2& start, const glm::vec2& end, const glm::vec2& center, const GLfloat& radius)
{
	glm::vec2 dir = end - start;	// start to end
	glm::vec2 f = start - center;	// center to start

	GLfloat a = glm::dot(dir, dir);
	GLfloat b = 2 * glm::dot(f, dir);
	GLfloat c = glm::dot(f, f) - (radius * radius);

	GLfloat discriminant = (b * b) - 4 * a * c;

	if (discriminant < 0)
	{
		return GL_FALSE; // no intersection
	}
	else
	{
		// ray didn't totally miss sphere, so there is a solution to the equation.
		discriminant = sqrtf(discriminant);

		// either solution may be on or off the ray so need to test both
		// t1 is always the smaller value, because BOTH discriminant and a are non-negative.
		GLfloat t1 = (-b - discriminant) / (2 * a);
		GLfloat t2 = (-b + discriminant) / (2 * a);

		if (t1 >= 0 && t1 <= 1)
		{
			return GL_TRUE;
		}
		if (t2 >= 0 && t2 <= 1)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

/* https://gamedev.stackexchange.com/questions/26004/how-to-detect-2d-line-on-line-collision */
GLboolean CheckCollision(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& u1, const glm::vec2& u2)
{
	GLfloat denominator = ((v2.x - v1.x) * (u2.y - u1.y)) - ((v2.y - v1.y) * (u2.x - u1.x));
	GLfloat numerator1 = ((v1.y - u1.y) * (u2.x - u1.x)) - ((v1.x - u1.x) * (u2.y - u1.y));
	GLfloat numerator2 = ((v1.y - u1.y) * (v2.x - v1.x)) - ((v1.x - u1.x) * (v2.y - v1.y));

	if (denominator == 0)
	{
		return (numerator1 == 0 && numerator2 == 0);
	}

	GLfloat r = numerator1 / denominator;
	GLfloat s = numerator2 / denominator;

	return (r >= 0 && r <= 1) && (s >= 0 && s <= 1);
}
/* https://www.geeksforgeeks.org/program-for-point-of-intersection-of-two-lines/ */
glm::vec2 GetLineIntersection(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& u1, const glm::vec2& u2)
{
	// Line AB represented as a1x + b1y = c1
	GLfloat a1 = v2.y - v1.y;
	GLfloat b1 = v1.x - v2.x;
	GLfloat c1 = a1 * (v1.x) + b1 * (v1.y);

	// Line CD represented as a2x + b2y = c2
	GLfloat a2 = u2.y - u1.y;
	GLfloat b2 = u1.x - u2.x;
	GLfloat c2 = a2 * (u1.x) + b2 * (u1.y);

	GLfloat determinant = a1 * b2 - a2 * b1;

	GLfloat x = (b2 * c1 - b1 * c2) / determinant;
	GLfloat y = (a1 * c2 - a2 * c1) / determinant;
	return glm::vec2(x, y);
}

GLboolean CheckCollision(const glm::vec2& v, const glm::vec2& u, const GLfloat& vRadius, const GLfloat& uRadius)
{
	GLfloat distance = glm::length(v - u);
	GLfloat radius = (vRadius + uRadius);

	if (distance <= radius)
	{
		return GL_TRUE;
	}

	return GL_FALSE;
}

/* https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection */
GLboolean CheckCollision(const GLrect& rect, const glm::vec2& v, const GLfloat& vRadius)
{
	glm::vec2 rectCenter = rect.GetCenter();
	GLfloat rectRadius = rect.GetRadius();

	if (::CheckCollision(rectCenter, v, rectRadius, vRadius) == GL_FALSE)
	{
		return GL_FALSE;
	}

	GLfloat rectHalfWidth = rect.GetWidth() / 2;
	GLfloat rectHalfHeight = rect.GetHeight() / 2;

	glm::vec2 circleDistance;

	circleDistance.x = fabs(v.x - rectCenter.x);
	circleDistance.y = fabs(v.y - rectCenter.y);

	if (circleDistance.x > rectHalfWidth + vRadius)
	{
		return GL_FALSE;
	}
	if (circleDistance.y > rectHalfHeight + vRadius)
	{
		return GL_FALSE;
	}

	if (circleDistance.x <= rectHalfWidth)
	{
		return GL_TRUE;
	}
	if (circleDistance.y <= rectHalfHeight)
	{
		return GL_TRUE;
	}

	GLfloat a = (v.x - rectHalfWidth);
	GLfloat b = (v.y - rectHalfHeight);
	GLfloat cornerDistance_sq = (a * a) + (b * b);

	return cornerDistance_sq <= (vRadius * vRadius);
}


GLboolean CheckCollision(const glm::vec3& vCylinderPos, const glm::vec3& uPoint, const GLfloat& vRadius, const GLfloat& uRadius, const GLfloat& vHeight)
{
	const glm::vec2 vCylinderCenter = ConvertVec2(vCylinderPos);
	const glm::vec2 uPointCenter = ConvertVec2(uPoint);
	if (CheckCollision(vCylinderCenter, uPointCenter, vRadius, uRadius) == GL_FALSE)
	{
		return GL_FALSE;
	}

	GLfloat minHeight = vCylinderPos.y;
	GLfloat maxHeight = minHeight + vHeight;
	if ((uPoint.y <= maxHeight) && (uPoint.y >= minHeight))
	{
		return GL_TRUE;
	}

	return GL_FALSE;
}

void SetConsoleCursor(short x, short y)
{
	COORD cursor = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
}


GLboolean FindEmptyCoreID(mutex& m, unordered_set<GLuint>& emptyCore, GLuint& id)
{
	if (emptyCore.empty() == GL_FALSE)
	{
		m.lock();
		id = *emptyCore.begin();
		emptyCore.erase(id);
		m.unlock();
		return GL_TRUE;
	}

	this_thread::yield();
	return GL_FALSE;
}

static GLboolean isGameOver = GL_FALSE;
GLvoid GameOver()
{
	isGameOver = GL_TRUE;
}
GLboolean IsGameOver()
{
	return isGameOver;
}