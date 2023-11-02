#pragma once
#include "stdafx.h"

#define PLAYER_RADIUS 10 // Width = 20 : 2m -> radius = 1m
#define PLAYER_HEIGHT 40 // height = 40 : 4m
#define PLAYER_WALK_SPEED 50 // 5m/s
#define PLAYER_RUN_SPEED 100 // 10m/s
#define PLAYER_JUMP_SPEED 30 // 3 m/s

#define RUN_SOUND_TERM 0.5 // 1초에 1번씩


class SharedObject;
class Camera;
class Cuboid;
class TriangleObject;
class Player;
class Circle;
class Gun;
enum class GunType;

class Rifle;
class ShotGun;
class Sniper;
class Launcher;

namespace playerState {
	/* 기본 State  */
	class PlayerState abstract {
	protected:
		Player* mPlayer = nullptr;
	public:
		PlayerState(Player* player) { mPlayer = player; }
		virtual GLvoid Enter(const Event& e = Event::None, const GLint& value = 0) abstract;
		virtual GLvoid Exit() abstract;
		virtual GLvoid Update() abstract;
		virtual GLvoid HandleEvent(const Event& e, const GLint& key) abstract;
	};

	class Idle : public PlayerState {
	public:
		Idle(Player* player) : PlayerState(player) {};
		GLvoid Enter(const Event& e = Event::None, const GLint& value = 0) override;
		GLvoid Exit() override;
		GLvoid Update() override;
		GLvoid HandleEvent(const Event& e, const GLint& key) override;
	};

	class Walk : public PlayerState {
	public:
		Walk(Player* player) : PlayerState(player) {};
		GLvoid Enter(const Event& e = Event::None, const GLint& value = 0) override;
		GLvoid Exit() override;
		GLvoid Update() override;
		GLvoid HandleEvent(const Event& e, const GLint& key) override;
	};

	class Jump : public PlayerState {
	private:
		const GLfloat jumpTime = 0.5f; // jump up N sec
		GLfloat t = 0;

		GLboolean isKeyUp = GL_FALSE;
	public:
		Jump(Player* player) : PlayerState(player) {};
		GLvoid Enter(const Event& e = Event::None, const GLint& value = 0) override;
		GLvoid Exit() override;
		GLvoid Update() override;
		GLvoid HandleEvent(const Event& e, const GLint& key) override;
	};
}



class Player {
private:
	// state
	playerState::PlayerState* mCrntState = nullptr;

	// direction
	GLchar mDirX = 0;
	GLchar mDirY = 0;
	GLchar mDirZ = 0;

	// position
	glm::vec3 mPosition = { 0, 0, 0 };

	// bodies
	SharedObject* mHead = nullptr;
	SharedObject* mBody = nullptr;
	SharedObject* mArms = nullptr;
	SharedObject* mLegL = nullptr;
	SharedObject* mLegR = nullptr;
	GLfloat mLegRotation = 0.0f;
	GLint mLegDir = LEFT;

	// guns
	Gun* mCrntGun = nullptr;
	Rifle* mRifle = nullptr;
	Sniper* mSniper = nullptr;
	ShotGun* mShotGun = nullptr;
	Launcher* mLauncher = nullptr;

	// camera
	const CameraMode* mCameraMode = nullptr;
	Camera* mFpCamera = nullptr;
	Camera* mTpCamera = nullptr;
	glm::vec3 mTpCameraPosition = { 0, 0, 0 };
	GLfloat mTpCameraPitch = 0.0f;
	GLfloat mTPCameraMaxYaw = 45.0f;

	Camera* mZoomFPCamera = nullptr;

	// values
	GLfloat mHP = 100.0f;
	GLfloat mSpeed = PLAYER_WALK_SPEED;
	GLfloat mJumpSpeed = PLAYER_JUMP_SPEED;
	GLint mHoldTurret = 0;
	GLfloat mFrameTime = 0.0f;

	// for collision
	Circle* mBoundingCircle = nullptr;
	GLfloat mFloor = 0.0f;
	GLfloat mTop = 0.0f;
	GLfloat mYtop = 0.0f;
	GLfloat mYbot = 0.0f;

	// rotation
	GLfloat mYaw = 0.0f;
	GLfloat mPitch = 0.0f;

public:
	Player(const glm::vec3& position, const CameraMode* cameraMode);
	~Player();

	Cuboid* mCuboid = nullptr;

	// state
	enum class State { Idle = 0, Walk, Run, Jump };
	GLvoid ChangeState(const State& playerState, const Event& e = Event::None, const GLint& value = 0);

	// Frame
	GLvoid Update();
	GLvoid Draw(const CameraMode& cameraMode) const;
	GLvoid DrawIcon() const;

	// Process
	GLvoid ProcessKeyDown(const GLint& key);
	GLvoid ProcessKeyUp(const GLint& key);
	GLvoid ProcessMouse(GLint button, GLint state, GLint x, GLint y);

	// Movement
	GLvoid Move();
	GLvoid Stop();
	inline constexpr GLvoid Run() { mSpeed = PLAYER_RUN_SPEED; };
	inline constexpr GLvoid StopRun() { mSpeed = PLAYER_WALK_SPEED; };
	GLvoid AddDir(const GLint& key);
	GLvoid SubDir(const GLint& key);
	GLvoid SetDir(const GLint& key, const GLint& value);
	inline constexpr GLchar GetDirX() const { return mDirX; }
	inline constexpr GLchar GetDirY() const { return mDirY; }
	inline constexpr GLchar GetDirZ() const { return mDirZ; }

	// Rotation
	GLvoid Rotate(const GLfloat& yaw, const GLfloat& pitch, const GLfloat& roll);
	GLvoid RotateLeg();
	GLvoid ReleaseLegRotation();

	// Variables
	const glm::vec3* GetRefPos() const { return &mPosition; }
	glm::vec3 GetPosition() const;
	inline constexpr Camera* GetFirstPersonCamera() { return mFpCamera; }
	inline constexpr Camera* GetThirdPersonCamera() { return mTpCamera; }
	inline constexpr Camera* GetZoomFirstPersonCamera() { return mZoomFPCamera; }
	GLint GetAmmo() const;
	GLint GetMaxAmmo() const;
	GLint GetHoldTullet() const;
	GLfloat GetRadius() const;
	GLvoid Damage(const GLfloat& damage);
	GLfloat GetHp() const;
	GunType GetGunType() const;


	// Add
	GLvoid AddHoldturret(const GLint& value);


	// Skills
	GLvoid Install_Turret();
	GLvoid ChaingeGun();
};