#include "stdafx.h"
#include "Player.h"
#include "Object.h"
#include "Camera.h"
#include "Timer.h"
#include "Map.h"
#include "Gun.h"
#include "Building.h"
#include "Sound.h"
#include "Turret.h"

#define STB_IMAGE_IMPLEMENTATION
#include <myGL/stb_image.h>
// extern
extern Map* crntMap;
extern BuildingManager* buildingManager;
extern TurretManager* turretManager;

using namespace playerState;

// movement key sets
const set<GLint> movFB = { 'w', 'W', 's', 'S' };
const set<GLint> movLR = { 'a', 'A', 'd', 'D' };
const set<GLint> movKeys = { 'w', 'W', 's', 'S', 'a', 'A', 's', 'S', 'd', 'D' };

////////////////////////////// [ State ] //////////////////////////////
/********** [ IDLE ] **********/
GLvoid Idle::Enter(const Event& e, const GLint& value)
{
	mPlayer->Stop();
}
GLvoid Idle::Exit()
{
	
}
GLvoid Idle::Update()
{
	mPlayer->ReleaseLegRotation();
	mPlayer->Move();
}

GLvoid Idle::HandleEvent(const Event& e, const GLint& key)
{
	switch (e)
	{
	case Event::KeyDown:
		if(movKeys.find(key) != movKeys.end())
		{
			mPlayer->ChangeState(Player::State::Walk, e, key);
		}
		else if (key == KEY_SPACEBAR)
		{
			mPlayer->ChangeState(Player::State::Jump);
		}
		break;
	case Event::KeyUp:
		if (movKeys.find(key) != movKeys.end())
		{
			mPlayer->ChangeState(Player::State::Walk, Event::KeyUp, key);
		}
		break;
	}
}

/********** [ WALK ] **********/
GLvoid Walk::Enter(const Event& e, const GLint& value)
{
	switch (e)
	{
	case Event::KeyDown:
		mPlayer->AddDir(value);
		break;
	case Event::KeyUp:
		mPlayer->SubDir(value);
		break;
	default:
		break;
	}
}
GLvoid Walk::Exit()
{
}
GLvoid Walk::Update()
{
	mPlayer->Move();
}
GLvoid Walk::HandleEvent(const Event& e, const GLint& key)
{
	switch (e)
	{
	case Event::KeyDown:
		if (movKeys.find(key) != movKeys.end())
		{
			mPlayer->AddDir(key);
		}

		if (mPlayer->GetDirX() == 0 && mPlayer->GetDirZ() == 0)
		{
			mPlayer->ChangeState(Player::State::Idle, e, key);
		}
		else if (key == KEY_SPACEBAR)
		{
			mPlayer->ChangeState(Player::State::Jump);
		}
		else if (key == GLUT_KEY_SHIFT_L)
		{
			mPlayer->Run();
		}

		break;
	case Event::KeyUp:
		if (movKeys.find(key) != movKeys.end())
		{
			mPlayer->SubDir(key);
		}

		if (mPlayer->GetDirX() == 0 && mPlayer->GetDirZ() == 0)
		{
			mPlayer->ChangeState(Player::State::Idle, e, key);
		}
		else if (key == GLUT_KEY_SHIFT_L)
		{
			mPlayer->StopRun();
		}
		break;
	}
}


/********** [ JUMP ] **********/
GLvoid Jump::Enter(const Event& e, const GLint& value)
{
	t = 0;
	mPlayer->SetDir(KEY_SPACEBAR, UP);
}
GLvoid Jump::Exit()
{
}
GLvoid Jump::Update()
{
	t += timer::DeltaTime();
	if (t >= jumpTime)
	{
		if (mPlayer->GetDirY() == UP)
		{
			t = 0;
			mPlayer->SetDir(KEY_SPACEBAR, DOWN);
		}
		else
		{
			if (isKeyUp)
			{
				mPlayer->SetDir(KEY_SPACEBAR, 0);
				if (mPlayer->GetDirX() == 0 && mPlayer->GetDirZ() == 0)
				{
					mPlayer->ChangeState(Player::State::Idle);
				}
				else
				{
					mPlayer->ChangeState(Player::State::Walk);
				}
			}
			else
			{
				mPlayer->ChangeState(Player::State::Jump);
			}
		}
		return;
	}
	mPlayer->Move();
}
GLvoid Jump::HandleEvent(const Event& e, const GLint& key)
{
	if (key == KEY_SPACEBAR)
	{
		if (e == Event::KeyUp)
		{
			isKeyUp = GL_TRUE;
		}
		else if (e == Event::KeyDown)
		{
			isKeyUp = GL_FALSE;
			if (key == GLUT_KEY_SHIFT_L)
			{
				mPlayer->StopRun();
			}
		}
		return;
	}

	if (movKeys.find(key) != movKeys.end())
	{
		switch (e)
		{
		case Event::KeyDown:
			mPlayer->AddDir(key);
			break;
		case Event::KeyUp:
			mPlayer->SubDir(key);
			break;
		}
	}
}





////////////////////////////// [ Player ] //////////////////////////////
Player::Player(const glm::vec3& position)
{
	mPosition = position;
	mTpCameraPosition = position;
	mHead = new SharedObject(GetIdentityTextureObject(Textures::Player_Head));
	mBody = new SharedObject(GetIdentityTextureObject(Textures::Player_Body));
	mArms = new SharedObject(GetIdentityTextureObject(Textures::Player_Arms));
	mLegL = new SharedObject(GetIdentityTextureObject(Textures::Player_Leg_L));
	mLegR = new SharedObject(GetIdentityTextureObject(Textures::Player_Leg_R));

	mHead->SetPivot(mBody->GetRefPos());
	mArms->SetPivot(mBody->GetRefPos());
	mLegL->SetPivot(mBody->GetRefPos());
	mLegR->SetPivot(mBody->GetRefPos());

	mHead->MoveY(31, GL_FALSE);
	mArms->MoveY(28, GL_FALSE);
	mLegL->MoveY(16, GL_FALSE);
	mLegR->MoveY(16, GL_FALSE);

	mFpCamera = new Camera();
	mFpCamera->SetPivot(&mPosition);
	mFpCamera->SetPosY(38);
	mFpCamera->SetFovY(110.0f);
	mFpCamera->SetLook(mBody->GetLook());

	mTpCamera = new Camera();
	mTpCamera->SetPivot(&mPosition);
	mTpCamera->SetPosY(100);
	mTpCamera->SetPosZ(-75);
	mTpCamera->SetFovY(110.0f);
	mTpCamera->Look(mBody->GetPosition());


	mZoomFPCamera = new Camera();
	mZoomFPCamera->SetPivot(&mPosition);
	mZoomFPCamera->SetPosY(38);
	mZoomFPCamera->SetPosZ(100);
	mZoomFPCamera->SetFovY(110.0f);
	mZoomFPCamera->SetLook(mBody->GetLook());

	mHead->SetRotationPivot(mFpCamera->GetRefPos());
	mArms->SetRotationPivot(mFpCamera->GetRefPos());

	glm::vec3 gunPosition = glm::vec3(-PLAYER_RADIUS + 1.0f, mFpCamera->GetPviotedPosition().y - 18, 0);
	
	mRifle = new Rifle(gunPosition, &mPosition);
	mSniper = new Sniper(gunPosition, &mPosition);
	mShotGun = new ShotGun(gunPosition, &mPosition);
	mLauncher = new Launcher(gunPosition, &mPosition);


	mCrntGun = mRifle;
	mBoundingCircle = new Circle(mBody->GetRefPos(), PLAYER_RADIUS, GL_TRUE);
	mBoundingCircle->SetColor(BLUE);

	Rotate(0, 180, 0);

	mHP = 50.f;
	ChangeState(State::Idle);
}
Player::~Player()
{
	delete mHead;
	delete mBody;
	delete mArms;
	delete mLegL;
	delete mLegR;

	delete mFpCamera;
	delete mTpCamera;
}



GLvoid Player::AddDir(const GLint& key)
{
	switch (key)
	{
	case 'w':
	case 'W':
		mDirZ += FRONT;
		break;
	case 's':
	case 'S':
		mDirZ += BACK;
		break;
	case 'a':
	case 'A':
		mDirX += LEFT;
		break;
	case 'd':
	case 'D':
		mDirX += RIGHT;
		break;
	default:
		break;
	}
}
GLvoid Player::SubDir(const GLint& key)
{
	switch (key)
	{
	case 'w':
	case 'W':
		mDirZ -= FRONT;
		break;
	case 's':
	case 'S':
		mDirZ -= BACK;
		break;
	case 'a':
	case 'A':
		mDirX -= LEFT;
		break;
	case 'd':
	case 'D':
		mDirX -= RIGHT;
		break;
	default:
		break;
	}
}
GLvoid Player::SetDir(const GLint& key, const GLint& value)
{
	switch (key)
	{
	case KEY_SPACEBAR:
		mDirY = value;
		break;
	}
}

GLvoid Player::ChangeState(const State& playerState, const Event& e, const GLint& value)
{
	if (mCrntState != nullptr)
	{
		mCrntState->Exit();
		delete mCrntState;
	}

	switch (playerState)
	{
	case State::Idle:
		mCrntState = new Idle(this);
		break;
	case State::Walk:
		mCrntState = new Walk(this);
		break;
	case State::Jump:
		mCrntState = new Jump(this);
		break;
	default:
		assert(0);
	}

	mCrntState->Enter(e, value);
}

GLvoid Player::Update()
{
	mCrntState->Update();
	mCrntGun->SetYaw(mYaw);
	mCrntGun->SetPitch(mPitch);
	if (mlsFire) mCrntGun->StartFire();
	else mCrntGun->StopFire();
	mPosition = mBody->GetPviotedPosition();
	mCrntGun->Update();
}

GLvoid Player::Draw(const CameraMode& cameraMode) const
{
	if (cameraMode == CameraMode::FirstPerson)
	{
		mCrntGun->Draw();
		return;
	}

	mHead->Draw();
	mBody->Draw();
	mArms->Draw();
	mLegL->Draw();
	mLegR->Draw();
	mCrntGun->Draw();
	mBoundingCircle->Draw();
}
GLvoid Player::DrawIcon() const
{
}

GLvoid Player::ProcessKeyDown(const GLint& key)
{
	switch (key)
	{
	case 'r':
	case 'R':
		mCrntGun->Reload();
		return;
	case 'f':
	case 'F':
		Install_Turret();
		return;
	case 'q':
	case 'Q':
		ChaingeGun();
		return;
	}

	mCrntState->HandleEvent(Event::KeyDown, key);
}
GLvoid Player::ProcessKeyUp(const GLint& key)
{
	mCrntState->HandleEvent(Event::KeyUp, key);
}
GLvoid Player::ProcessMouse(GLint button, GLint state, GLint x, GLint y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			mCrntGun->StartFire();
		}
		else if (state == GLUT_UP)
		{
			mCrntGun->StopFire();
		}
		break;
	}
}

GLvoid Player::Move()
{
	GLfloat correction = 1.0f;
	if (mDirX != 0.0f && mDirZ != 0.0f)
	{
		correction = 0.8f;
	}

	glm::vec3 prevPos = mBody->GetPosition();
	if (mDirX != 0.0f) mBody->MoveX(mSpeed * mDirX * correction);
	if (mDirY != 0.0f) mBody->MoveY(mJumpSpeed * mDirY);
	if (mDirZ != 0.0f) mBody->MoveZ(mSpeed * mDirZ * correction);

	// xz collision
	if (crntMap->CheckCollision(mBoundingCircle) == GL_TRUE || buildingManager->CheckCollision(mBoundingCircle) == GL_TRUE)
	{
		mBody->SetPosX(prevPos.x);
		mBody->SetPosZ(prevPos.z);
	}


	RotateLeg();
}
GLvoid Player::Stop()
{
	mDirX = 0;
	mDirZ = 0;
}

GLvoid Player::Rotate(const GLfloat& yaw, const GLfloat& pitch, const GLfloat& roll)
{
	mYaw += yaw;
	mPitch += pitch;
	if (fabs(mYaw) > 89.0f)
	{
		mYaw = 89.0f * GetSign(mYaw);
	}

	mBody->RotateLocal(0, pitch, 0);
	mHead->SetLook(mBody->GetLook());
	mArms->SetLook(mBody->GetLook());
	mLegL->SetLook(mBody->GetLook());
	mLegR->SetLook(mBody->GetLook());

	mHead->RotateLocal(mYaw, 0, 0);
	mArms->RotateLocal(mYaw, 0, 0);
	mLegR->RotateLocal(mLegRotation, 0, 0);
	mLegL->RotateLocal(-mLegRotation, 0, 0);

	mFpCamera->SetLook(mBody->GetLook());
	mFpCamera->RotateLocal(mYaw, 0, 0);

	mTpCamera->SetLook(mBody->GetLook());
	mTpCamera->SetPosition({ 0, 100, -50 });
	mTpCamera->RotatePosition({ 0,0,0 }, mTpCamera->GetUp(), mPitch);

	GLfloat tpCameraYaw = mYaw;
	constexpr GLfloat tpCameraAngle = 30.0f;
	if (tpCameraYaw > mTPCameraMaxYaw + tpCameraAngle)
	{
		tpCameraYaw = mTPCameraMaxYaw + tpCameraAngle;
	}
	else if (tpCameraYaw - tpCameraAngle < -70.0f)
	{
		tpCameraYaw = tpCameraAngle - 70.0f;
	}
	mTpCamera->RotatePosition(mHead->GetPosition(), mTpCamera->GetRight(), tpCameraYaw);
	mTpCamera->RotateLocal(tpCameraYaw - tpCameraAngle, 0, 0);
	
	/*
	if (*mCameraMode == CameraMode::FirstPerson)
	{
		mCrntGun->Rotate(mYaw, mPitch);
	}
	else
	{
		mCrntGun->RotateLocal(mYaw, mPitch);
	}
	*/

}
GLvoid Player::RotateLeg()
{
	// 1초에 maxLegRotation만큼 legRotationSpeed 배수로 회전
	constexpr GLfloat legRotaionSpeed = 4.0f;
	constexpr GLfloat maxLegRotation = 30.0f;
	GLfloat legRotation = timer::DeltaTime() * maxLegRotation * legRotaionSpeed;
	if (fabs(mLegRotation + legRotation) >= maxLegRotation)
	{
		mLegDir *= -1;
	}

	mLegRotation += legRotation * mLegDir;
	mLegR->SetLook(mBody->GetLook());
	mLegL->SetLook(mBody->GetLook());
	mLegR->RotateLocal(mLegRotation, 0, 0);
	mLegL->RotateLocal(-mLegRotation, 0, 0);
}
GLvoid Player::ReleaseLegRotation()
{
	if (fabs(mLegRotation) <= 0.1f)
	{
		return;
	}

	constexpr GLfloat maxLegRotation = 30.0f;
	GLfloat legRotaionSpeed = 4.0f * (mLegRotation * 0.01f);
	GLfloat legRotation = timer::DeltaTime() * maxLegRotation * legRotaionSpeed;

	if (mLegDir > 0)
	{
		mLegRotation -= legRotation * mLegDir;
	}
	else
	{
		mLegRotation += legRotation * mLegDir;
	}

	mLegR->SetLook(mBody->GetLook());
	mLegL->SetLook(mBody->GetLook());
	mLegR->RotateLocal(mLegRotation, 0, 0);
	mLegL->RotateLocal(-mLegRotation, 0, 0);
}

glm::vec3 Player::GetPosition() const
{
	return mBody->GetPosition();
}

glm::vec3 Player::GetBodyLook() const
{
	return mBody->GetLook();
}

glm::vec3 Player::GetHeadLook() const
{
	return mHead->GetLook();
}

glm::vec3 Player::GetGunPos() const
{
	return mCrntGun->GetPosition();
}

glm::vec3 Player::GetGunLook() const
{
	return mCrntGun->GetLook();
}

glm::quat Player::GetGunRotation() const
{
	return mCrntGun->GetRotation(); 
}

GLint Player::GetAmmo() const
{
	return mCrntGun->GetAmmo();
}

GLint Player::GetMaxAmmo() const
{
	return mCrntGun->GetMaxAmmo();
}

Gun* Player::GetGun()
{
	if (mCrntGun != nullptr) return mCrntGun;
}

GunType Player::GetGunType() const
{
	if (mCrntGun != nullptr) return mCrntGun->GetType();
	else return GunType::None;
}


GLint Player::GetHoldTullet() const
{
	return mHoldTurret;
}

GLvoid Player::Damage(const GLfloat& damage)
{
	mHP -= damage;
	if (mHP <= 0)
	{
		GameOver();
	}
}

GLfloat Player::GetRadius() const
{
	return PLAYER_RADIUS;
}

GLfloat Player::GetHp() const
{
	return mHP;
}



GLvoid Player::Install_Turret()
{
	glm::vec3 position = GetPosition();
	turretManager->Create(glm::vec3( position.x, 0, position.z));
}

GLvoid Player::ChaingeGun()
{
	static int gun_num = 0;

	if (mCrntGun->IsReloading())
	{
		mCrntGun->Reload();
	}

	if (++gun_num > 3)
	{
		gun_num = 0;
	}

	Gun* prevGun = mCrntGun;
	switch (gun_num)
	{
	case 0:
		mCrntGun = mRifle;
		break;

	case 1:
		mCrntGun = mShotGun;
		break;

	case 2:
		mCrntGun = mLauncher;
		break;

	case 3:
		mCrntGun = mSniper;
		break;
	}

	mCrntGun->RotateLocal(prevGun->GetYaw(), prevGun->GetPitch());
}

GLvoid Player::PlayerSend(SOCKET& client_sock)
{
	int retval = 0;
	// ======= 사용자 정보 송신 ======
	char HPbuf[sizeof(GLfloat)];
	memcpy(HPbuf, &mHP, sizeof(HPbuf));
	retval = send(client_sock, HPbuf, sizeof(HPbuf), 0);
	// ======= ========== ======
	SetConsoleCursor(0, 12);
#ifdef  DEBUG
	cout << "SEND HP : " << mHP << endl;
#endif //  DEBUG

}

struct PlayerInfo {
	char pos[sizeof(uint32_t) * 3];
	char bodylook[sizeof(uint32_t) * 3];
	char headlook[sizeof(uint32_t) * 3];
	char isFired[sizeof(bool)];
	char isInstall[sizeof(bool)];
	char gunpos[sizeof(uint32_t) * 3];
	char gunlook[sizeof(uint32_t) * 3];
	char guntype[sizeof(GunType)];
	char gunrotate[sizeof(glm::quat)];
};

GLvoid Player::PlayerRecv(SOCKET& client_sock)
{
	// ======= 사용자 정보수신 ======
	PlayerInfo playerInfo;

	char buf[sizeof(PlayerInfo)];
	int retval = 0;
	uint32_t pos[3];
	uint32_t bodylook[3];
	uint32_t headlook[3];
	uint32_t nGunPos[3];
	uint32_t nGunLook[3];
	GunType gunType;
	glm::quat rotate;
	bool isFire , isInstall = false;

	retval = recv(client_sock, buf, sizeof(PlayerInfo), 0);
	memcpy(&playerInfo, buf, sizeof(PlayerInfo));
	memcpy(&pos, playerInfo.pos, sizeof(uint32_t) * 3);
	memcpy(&bodylook, playerInfo.bodylook, sizeof(uint32_t) * 3);
	memcpy(&headlook, playerInfo.headlook, sizeof(uint32_t) * 3);
	memcpy(&isFire, playerInfo.isFired, sizeof(bool));
	memcpy(&isInstall, playerInfo.isInstall, sizeof(bool));
	memcpy(nGunPos, playerInfo.gunpos, sizeof(uint32_t) * 3);
	memcpy(nGunLook, playerInfo.gunlook, sizeof(uint32_t) * 3);
	memcpy(&gunType, playerInfo.guntype, sizeof(GunType));
	memcpy(&rotate, playerInfo.gunrotate, sizeof(glm::quat));

	glm::vec3 playerPos;
	glm::vec3 playerBodyLook;
	glm::vec3 playerHeadLook;
	glm::vec3 fGunPos;
	glm::vec3 fGunLook;
	playerPos.x = *reinterpret_cast<float*>(&pos[0]);
	playerPos.y = *reinterpret_cast<float*>(&pos[1]);
	playerPos.z = *reinterpret_cast<float*>(&pos[2]);
	playerBodyLook.x = *reinterpret_cast<float*>(&bodylook[0]);
	playerBodyLook.y = *reinterpret_cast<float*>(&bodylook[1]);
	playerBodyLook.z = *reinterpret_cast<float*>(&bodylook[2]);
	playerHeadLook.x = *reinterpret_cast<float*>(&headlook[0]);
	playerHeadLook.y = *reinterpret_cast<float*>(&headlook[1]);
	playerHeadLook.z = *reinterpret_cast<float*>(&headlook[2]);
	fGunPos.x = *reinterpret_cast<float*>(&nGunPos[0]);
	fGunPos.y = *reinterpret_cast<float*>(&nGunPos[1]);
	fGunPos.z = *reinterpret_cast<float*>(&nGunPos[2]);
	fGunLook.x = *reinterpret_cast<float*>(&nGunLook[0]);
	fGunLook.y = *reinterpret_cast<float*>(&nGunLook[1]);
	fGunLook.z = *reinterpret_cast<float*>(&nGunLook[2]);
	SetGunType(gunType);
	SetPosition(playerPos);
	SetBodyLook(playerBodyLook);
	SetHeadLook(playerHeadLook);
	SetGunPos(fGunPos);
	SetGunLook(fGunLook);
	SetGunRotation(rotate);
	mIsInstall = isInstall;
	mlsFire = isFire;


	if (mIsInstall) Install_Turret();
	mIsInstall = false;
}

GLvoid Player::SetGunType(GunType gunType)
{
	switch (gunType)
	{
	case GunType::Rifle:
		mCrntGun = mRifle;
		break;

	case GunType::Shotgun:
		mCrntGun = mShotGun;
		break;

	case GunType::Launcher:
		mCrntGun = mLauncher;
		break;

	case GunType::Sniper:
		mCrntGun = mSniper;
		break;
	}
}

GLvoid Player::SetGunRotation(glm::quat newRotate)
{
	mCrntGun->SetRotation(newRotate);
}

GLvoid Player::SetPosition(glm::vec3 newPos)
{
	mBody->SetPosition(newPos);
}

GLvoid Player::SetBodyLook(glm::vec3 newPos)
{
	mBody->SetLook(newPos);
}

GLvoid Player::SetHeadLook(glm::vec3 newPos)
{
	mHead->SetLook(newPos);
}

GLvoid Player::SetGunPos(glm::vec3 newPos)
{
	mCrntGun->SetPosition(newPos);
}

GLvoid Player::SetGunLook(glm::vec3 newPos)
{
	mCrntGun->SetLook(newPos);
}

GLvoid Player::AddHoldturret(const GLint& value)
{ 
	mHoldTurret += value;
}