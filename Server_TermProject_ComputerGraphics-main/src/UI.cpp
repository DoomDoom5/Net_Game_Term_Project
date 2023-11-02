#include "stdafx.h"
#include "UI.h"
#include "Object.h"
#include "Model.h"
#include "Player.h"
#include "Gun.h"
#include "Wave.h"

extern WaveManager* waveManager;
UIManager::UIManager()
{
	const Model* planeModel = GetModel(Models::Plane);
	mPlane = new ModelObject(planeModel, Shader::Back);

	// create max_ammo text
	mAmmo_Tens.texture = Textures::UI_NUM_1;
	mAmmo_Units.texture = Textures::UI_NUM_0;

	mMax_ammo_Tens.pos = glm::vec2(0.8f, -0.6f);
	mMax_ammo_Units.pos = glm::vec2(0.83f, -0.6f);
	mMax_ammo_Tens.scale = 0.005f;
	mMax_ammo_Units.scale = 0.005f;

	// create current_ammo text
	mAmmo_Tens.texture = Textures::UI_NUM_5;
	mAmmo_Units.texture = Textures::UI_NUM_0;

	mAmmo_Tens.pos = glm::vec2(0.7f, -0.6f);
	mAmmo_Units.pos = glm::vec2(0.73f, -0.6f);
	mAmmo_Tens.scale = 0.005f;
	mAmmo_Units.scale = 0.005f;

	// create slash_text
	mslash_text.texture = Textures::UI_TEXT_SLASH;
	mslash_text.pos = glm::vec2(0.765f, -0.6f);
	mslash_text.scale = 0.005f;

	// create HP text
	mhp_text.texture = Textures::UI_TEXT_HP;
	mhp_text.pos = glm::vec2(-0.65f, -0.65f);
	mhp_text.scale = 0.01f;

	// create HP_Bar
	mhp_bar.texture = Textures::UI_COLOR_HP;
	mhp_bar.pos = glm::vec2(-0.6f, -0.7f);
	mhp_bar.scale = 0.005f;

	// create mGun_symble
	mgun_symbol.texture = Textures::UI_SHOTGUN_SYMBOL;
	mgun_symbol.pos = glm::vec2(0.6f, -0.6f);
	mgun_symbol.scale = 0.01f;

	// create mTurret_symbol
	mTurret_symbol.texture = Textures::UI_TURRET_SYMBOL;
	mTurret_symbol.pos = glm::vec2(-0.78f, 0.8f);
	mTurret_symbol.scale = 0.01f;

	// create m X Text
	mText_X.texture = Textures::UI_TEXT_X;
	mText_X.pos = glm::vec2(-0.7f, 0.8f);
	mText_X.scale = 0.01f;

	// create HoldTurret
	mHoldTurret.pos = glm::vec2(-0.64f, 0.8f);
	mHoldTurret.scale = 0.01f;

	mGameOver.texture = Textures::UI_GAME_OVER;
	mGameOver.pos = glm::vec2(0, 0);
	mGameOver.scale = 0.2f;

	mWaves_Tens.texture = Textures::UI_NUM_0;
	mWaves_Tens.pos = glm::vec2(-0.08f, -0.22f);
	mWaves_Tens.scale = 0.025f;

	mWaves_Units.texture = Textures::UI_NUM_0;
	mWaves_Units.pos = glm::vec2(0.08f, -0.22f);
	mWaves_Units.scale = 0.025f;
}

GLvoid UIManager::DrawPlane(const UITexture& texture)
{
	mPlane->SetPosition(glm::vec3(texture.pos.x, texture.pos.y, 0));
	mPlane->SetTexture(texture.texture);
	mPlane->SetScale(texture.scale);
	mPlane->Draw();
}



GLvoid UIManager::Draw()
{
	GLint ammo = mPlayer->GetAmmo();
	GLint ammo_ten = static_cast<GLint>(ammo * 0.1f);
	GLint ammo_Unit = ammo % 10;

	mAmmo_Tens.texture = static_cast<Textures>(static_cast<GLint>(Textures::UI_NUM_0) + ammo_ten);
	mAmmo_Units.texture = static_cast<Textures>(static_cast<GLint>(Textures::UI_NUM_0) + ammo_Unit);

	GLint max_ammo = mPlayer->GetMaxAmmo();
	GLint max_ammo_ten = static_cast<GLint>(max_ammo * 0.1f);
	GLint max_ammo_units = max_ammo % 10;

	mMax_ammo_Tens.texture = (static_cast<Textures>(static_cast<GLint>(Textures::UI_NUM_0) + max_ammo_ten));
	mMax_ammo_Units.texture = (static_cast<Textures>(static_cast<GLint>(Textures::UI_NUM_0) + max_ammo_units));


	GLint hold_turret = mPlayer->GetHoldTullet();
	mHoldTurret.texture = (static_cast<Textures>(static_cast<GLint>(Textures::UI_NUM_0) + hold_turret));

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	DrawPlane(mAmmo_Tens);
	DrawPlane(mAmmo_Units);
	DrawPlane(mMax_ammo_Tens);
	DrawPlane(mMax_ammo_Units);
	DrawPlane(mslash_text);
	DrawPlane(mTurret_symbol);
	DrawPlane(mText_X);
	DrawPlane(mHoldTurret);

	GLfloat ui_HP = mPlayer->GetHp();
	DrawPlane(mhp_text);
	if (ui_HP > 0)
	{
	for (GLuint i = 0; i < (GLuint)ui_HP/10; i++)
	{
		mhp_bar.pos = glm::vec2(-0.6 + i * (0.03),-0.7);
		DrawPlane(mhp_bar);
	}
	}
	GunType type = mPlayer->GetGunType();
	switch (type)
	{
	case GunType::Rifle:
		mgun_symbol.texture = Textures::UI_RIFLE_SYMBOL;
		break;
	case GunType::Launcher:
		mgun_symbol.texture = Textures::UI_LAUNCHER_SYMBOL;
		break;
	case GunType::Shotgun:
		mgun_symbol.texture = Textures::UI_SHOTGUN_SYMBOL;
		break;
	case GunType::Sniper:
		mgun_symbol.texture = Textures::UI_SNIPER_SYMBOL;
		break;
	case GunType::White:
		break;
	default:
		break;
	}

	DrawPlane(mgun_symbol);

	if (IsGameOver())
	{
		GLint wave = waveManager->GetWave();
		GLint wave_ten = static_cast<GLint>(wave * 0.1f);
		GLint wave_Unit = wave % 10;

		mWaves_Tens.texture = static_cast<Textures>(static_cast<GLint>(Textures::UI_NUM_0) + wave_ten);
		mWaves_Units.texture = static_cast<Textures>(static_cast<GLint>(Textures::UI_NUM_0) + wave_Unit);

		DrawPlane(mGameOver);
		DrawPlane(mWaves_Tens);
		DrawPlane(mWaves_Units);
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}
