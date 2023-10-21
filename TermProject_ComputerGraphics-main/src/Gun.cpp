#include "stdafx.h"
#include "Gun.h"
#include "Object.h"
#include "Bullet.h"
#include "Timer.h"
#include "Sound.h"

extern SoundManager* soundManager;
extern BulletManager* bulletManager;

Gun::Gun(const glm::vec3& gunPosition, const glm::vec3* pivot)
{
	mObject = new SharedObject(GetIdentityTextureObject(Textures::Gun));

	mObject->SetPivot(pivot);
	mObject->SetPosition(gunPosition);

	mGunPosition = gunPosition;
}

GLvoid Gun::Update()
{
	if (IsReloading())
	{
		mCrntReloadTime -= timer::DeltaTime();
		if (IsReloading() == GL_FALSE)
		{
			mAmmo = mMaxAmmo;
			soundManager->PlayEffectSound(EffectSound::Reload, 0.2f, GL_TRUE);
		}
		return;
	}
	else if (mIsFire == GL_FALSE)
	{
		return;
	}

	mCrntFireDelay += timer::DeltaTime();
	if (mCrntFireDelay < mFireDelay)
	{
		return;
	}

	if (--mAmmo <= 0)
	{
		Reload();
	}
	 

	mCrntFireDelay = 0.0f;
	Shot();
}

GLvoid Gun::Draw()
{
	mObject->Draw();
}
GLvoid Gun::Rotate(const GLfloat& yaw, const GLfloat& pitch)
{
	mYaw = yaw;
	mPitch = pitch;

	mObject->ResetRotation();
	mObject->ResetLook();
	mObject->SetPosition(mGunPosition);
	mObject->RotatePosition({ mGunPosition.x , mGunPosition.y + 20 , 0 }, Vector3::Right(), yaw);

	mObject->Rotate(Vector3::Up(), mPitch);
	mObject->RotateLocal(mYaw, 0, 0);
}
GLvoid Gun::RotateLocal(const GLfloat& yaw, const GLfloat& pitch)
{
	mYaw = yaw;
	mPitch = pitch;

	mObject->ResetRotation();
	mObject->ResetLook();
	mObject->SetPosition(mGunPosition);
	mObject->RotatePosition({ mGunPosition.x , mGunPosition.y + 10, 0 }, Vector3::Right(), yaw);

	mObject->Rotate(Vector3::Up(), mPitch);
	mObject->RotateLocal(mYaw, 0, 0);
}

GLvoid Gun::Shot()
{
	glm::vec3 origin = { 0, 9, 0 };
	glm::vec3 bulletPos = { 0, 9, 38 };

	MultiplyVector(mObject->GetTransform(), bulletPos);
	MultiplyVector(mObject->GetTransform(), origin);

	BulletData data;

	switch (mType)
	{
	case GunType::Rifle:
		bulletManager->Create(mBulletData, origin, bulletPos, mYaw, mPitch);
		soundManager->PlayFireSound(EffectSound::Normal_shot, 0.1f);
		break;
	case GunType::Launcher:
		bulletManager->Create(mBulletData, origin, bulletPos, mYaw, mPitch);
		soundManager->PlayFireSound(EffectSound::Launcher_shot, 0.2f);
		break;
	case GunType::Sniper:
		bulletManager->Create(mBulletData, origin, bulletPos, mYaw, mPitch);
		soundManager->PlayFireSound(EffectSound::Sniper_shot, 0.1f);
		break;
	default:
		assert(0);
		break;
	}
}


GLvoid Gun::Reload()
{
	soundManager->PlayEffectSound(EffectSound::EmptyBullet, 0.2f, GL_TRUE);
	mCrntReloadTime = mReloadTime;
}


Rifle::Rifle(const glm::vec3& gunPosition, const glm::vec3* pivot) : Gun(gunPosition, pivot)
{
	mType = GunType::Rifle;
	mMaxAmmo = 30;
	mAmmo = mMaxAmmo;
	mFireDelay = 0.1f;
	mReloadTime = 2.0f;

	mBulletData.type = BulletType::Normal;
	mBulletData.color = WHITE;
	mBulletData.scale = 0.1f;
	mBulletData.velocity = 300.0f;
	mBulletData.weight = 30.0f;
	mBulletData.damage = 20.0f;
	mBulletData.model = Models::LowSphere;
}

ShotGun::ShotGun(const glm::vec3& gunPosition, const glm::vec3* pivot) : Gun(gunPosition, pivot)
{
	mType = GunType::Shotgun;
	mMaxAmmo = 20; 
	mAmmo = mMaxAmmo; 
	mFireDelay = 0.2f;
	mReloadTime = 1.0f;
	
	mBulletData.type = BulletType::Normal;
	mBulletData.color = BLUE;
	mBulletData.scale = 0.05f;
	mBulletData.velocity = 200.0f;
	mBulletData.weight = 100.0f;
	mBulletData.damage = 5.0f;
	mBulletData.model = Models::LowSphere;
}

GLvoid ShotGun::Shot() 
{
	glm::vec3 origin = { 0, 9, 0 };
	glm::vec3 bulletPos = { 0, 9, 38 };

	MultiplyVector(mObject->GetTransform(), bulletPos);
	MultiplyVector(mObject->GetTransform(), origin);
	bulletManager->Create(mBulletData, origin, bulletPos, mYaw, mPitch);
	for (GLint i = 0; i < mBuckbullets; i++)
	{
		GLfloat m_b_angle = mBuckAngle - (i * mBuckAngle/mBuckbullets * 2);
		bulletManager->Create(mBulletData, origin, bulletPos, mYaw, mPitch + m_b_angle);
		// mPitch + m_b_angle 부터 mPitch - m_b_angle 까지 mBuckbullets 만큼 발사
	}

	soundManager->PlayFireSound(EffectSound::Shotgun_shot, 0.2f);
}

Sniper::Sniper(const glm::vec3& gunPosition, const glm::vec3* pivot) : Gun(gunPosition, pivot)
{
	mType = GunType::Sniper;
	mMaxAmmo = 8;
	mAmmo = mMaxAmmo;
	mFireDelay = 0.5f;
	mReloadTime = 3.0f;

	mBulletData.type = BulletType::Normal;
	mBulletData.color = GREEN;
	mBulletData.scale = 0.25f;
	mBulletData.velocity = 600.0f;
	mBulletData.weight = 20.0f;
	mBulletData.damage = 80.0f;
	mBulletData.model = Models::LowSphere;
}

Launcher::Launcher(const glm::vec3& gunPosition, const glm::vec3* pivot) : Gun(gunPosition, pivot)
{
	mType = GunType::Launcher;
	mMaxAmmo = 5;
	mAmmo = mMaxAmmo;
	mFireDelay = 0.4f;
	mReloadTime = 4.0f;

	mBulletData.type = BulletType::Rocket;
	mBulletData.color = RED;
	mBulletData.scale = 1.5f;
	mBulletData.velocity = 300.0f;
	mBulletData.weight = 100.0f;
	mBulletData.damage = 150.0f;
	mBulletData.model = Models::GeoSphere;
}