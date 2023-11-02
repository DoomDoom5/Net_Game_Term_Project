#include "stdafx.h"

class Player;
class ModelObject;
enum class Textures;;

struct UITexture {
	Textures texture;
	glm::vec2 pos = glm::vec2(0, 0);
	GLfloat scale = 0.0f;
};

class UIManager {
private:
	ModelObject* mPlane = nullptr;
	/* Ammo */

	UITexture mMax_ammo_Tens;
	UITexture mMax_ammo_Units;

	UITexture mAmmo_Tens;
	UITexture mAmmo_Units;

	UITexture mhp_text;
	UITexture mhp_bar;

	UITexture mslash_text;
	UITexture mgun_symbol;
	UITexture mTurret_symbol;

	UITexture mText_X;
	UITexture mHoldTurret;
	const Player* mPlayer = nullptr;

	UITexture mWaves_Tens;
	UITexture mWaves_Units;
	UITexture mGameOver;
public:
	UIManager();

	GLvoid Draw();
	GLvoid DrawPlane(const UITexture& texture);
	GLvoid SetAmmo(const GLint& num);
	GLvoid SetHP(const GLfloat& HP);
	inline constexpr GLvoid SetPlayer(const Player* player)
	{
		mPlayer = player;
	}

};