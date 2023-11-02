#pragma once
#include "Bullet.h"

enum class GunType { None, Rifle, Launcher, Shotgun, Sniper, White };
struct BulletData;

class Gun {
protected:
	GunType mType = GunType::None;
	
	SharedObject* mObject = nullptr;
	glm::vec3 mGunPosition = { 0,0,0 };

	GLboolean mIsFire = GL_FALSE;

	GLfloat mFireDelay = 0.0f;
	GLfloat mCrntFireDelay = 0.0f;

	GLfloat mYaw = 0.0f;
	GLfloat mPitch = 0.0f;
	GLfloat mVelocity = 0.0f;
	GLfloat mDamage = 0.0f;

	GLint mAmmo = 0;
	GLint mMaxAmmo = 0;
	GLfloat mReloadTime = 0.0f;
	GLfloat mCrntReloadTime = 0.0f;

	BulletData mBulletData;
public:
	Gun(const glm::vec3& gunPosition, const glm::vec3* pivot);

	GLvoid Update();
	virtual GLvoid Shot();
	GLvoid Draw();


	inline constexpr GLvoid StartFire() { mIsFire = GL_TRUE; }
	inline constexpr GLvoid StopFire() { mIsFire = GL_FALSE; }

	GLvoid Rotate(const GLfloat& yaw, const GLfloat& pitch);
	GLvoid RotateLocal(const GLfloat& yaw, const GLfloat& pitch);
	inline constexpr GLint GetAmmo() const { return mAmmo; };
	inline constexpr GLint GetMaxAmmo() const { return mMaxAmmo; };
	inline constexpr GunType GetType() const { return mType; };

	inline constexpr GLfloat GetYaw() const { return mYaw; }
	inline constexpr GLfloat GetPitch() const { return mPitch; }

	GLvoid Reload();
	inline constexpr GLboolean IsReloading() const { return mCrntReloadTime > 0; }
};

class Rifle : public Gun
{
public:
	Rifle(const glm::vec3& gunPosition, const glm::vec3* pivot);
};

class ShotGun : public Gun
{
public:
	ShotGun(const glm::vec3& gunPosition, const glm::vec3* pivot);
	GLint mBuckbullets = 5;
	GLfloat mBuckAngle = 10.0f;

	GLvoid Shot();
};

class Sniper : public Gun
{
public:
	Sniper(const glm::vec3& gunPosition, const glm::vec3* pivot);
};

class Launcher : public Gun
{
public:
	Launcher(const glm::vec3& gunPosition, const glm::vec3* pivot);
};