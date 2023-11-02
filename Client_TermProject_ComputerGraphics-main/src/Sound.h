#pragma once

enum class BGMSound { Normal = 0, _count };

enum class EffectSound { Fire = 0, Hit, Damaged, Run, Reload, CheageWeapon, EmptyBullet,
	Drawing_ink, Drawing_Bigink,
	Normal_shot, Shotgun_shot, Launcher_shot, Sniper_shot ,Jump , 
	M_BlooperDead , M_EggDead, M_KoromonDead
	,Turret_install, Turret_FindEnemy ,_count };
enum class SoundChannel { Bgm = 0, Fire, Effect, Walk, _count };

// has Channel
constexpr GLuint NUM_SOUND_BGM = static_cast<GLuint>(BGMSound::_count);
constexpr GLuint NUM_SOUND_EFFECT = static_cast<GLuint>(EffectSound::_count);
constexpr GLuint NUM_SOUND_CHEENEL = static_cast<GLuint>(SoundChannel::_count);;

// Stop은 Play후에 사용할 것
class SoundManager {
private:
	typedef struct Sound {
		EffectSound effectSound{};
		glm::vec3 position{};
		GLfloat volume = 0.0f;
	}Sound;

	FMOD_SYSTEM* soundSystem = nullptr;
	FMOD_SOUND* bgmSounds[NUM_SOUND_BGM]{};
	FMOD_SOUND* effectSounds[NUM_SOUND_EFFECT]{};
	FMOD_CHANNEL* soundChannel[NUM_SOUND_CHEENEL]{};

public:
	SoundManager();
	
	/* volume을 수정할 경우 new channel을 True로 해야 함. */
	GLvoid PlayFireSound(const EffectSound& effectSound, const GLfloat& volume = 1.0f);
	GLvoid PlayWalkSound(const EffectSound& effectSound, const GLfloat& volume = 1.0f);
	GLvoid PlayEffectSound(const EffectSound& effectSound, const glm::vec3& position, const GLfloat& volume = 1.0f);
	GLvoid PlayEffectSound(const EffectSound& effectSound, const GLfloat& volume = 1.0f, GLboolean isNewChannel = GL_FALSE);
	GLvoid StopEffectSound();

	GLvoid PlayBGMSound(const BGMSound& bgmSound, const GLfloat& volume = 1.0f, GLboolean isNewChannel = GL_FALSE);
	GLvoid StopBGMSound();

};