#include "stdafx.h"
#include "Sound.h"
#include "Camera.h"
#include "Timer.h"

#define EFFECT_MAX_DISTANCE 600

extern const Camera* crntCamera;

inline constexpr GLvoid CheckResult(const FMOD_RESULT& result)
{
	if (result != FMOD_OK)
	{
		assert(0);
	}
}
SoundManager::SoundManager()
{
	FMOD_System_Create(&soundSystem, FMOD_VERSION);
	FMOD_System_Init(soundSystem, 32, FMOD_INIT_NORMAL, NULL);

	FMOD_RESULT result = FMOD_OK;

	// bgmSound
	result = FMOD_System_CreateSound(soundSystem, "Sounds\\bgm.mp3", FMOD_LOOP_NORMAL, 0, &bgmSounds[static_cast<GLint>(BGMSound::Normal)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\sound_hit.mp3", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::Hit)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\Action\\Action_run_snow(cut).mp3", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::Run)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\Action\\Action_jump.wav", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::Jump)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\Action\\Action_reload.wav", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::Reload)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\Action\\Action_Empty_bullet.wav", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::EmptyBullet)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\Action\\Action_WeaponChange.wav", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::CheageWeapon)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\fire\\gun_normal_fire_02(cut).mp3", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::Normal_shot)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\fire\\gun_normal_fire_02(cut).mp3", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::Normal_shot)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\fire\\gun_shotgun_fire(cut).wav", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::Shotgun_shot)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\fire\\gun_launcher_fire(cut).mp3", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::Launcher_shot)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\fire\\gun_Sniper_fire.mp3", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::Sniper_shot)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\bullet\\Bullet_middle_Ink_hit.wav", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::Drawing_ink)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\bullet\\Bullet_big_Ink_hit.wav", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::Drawing_Bigink)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\turret\\turret_Install.mp3", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::Turret_install)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\turret\\turret_FindEnemy.mp3", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::Turret_FindEnemy)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\Monster\\m_Blooper_dead.ogg", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::M_BlooperDead)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\Monster\\m_Egg_dead.ogg", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::M_EggDead)]);
	CheckResult(result);

	result = FMOD_System_CreateSound(soundSystem, "Sounds\\Monster\\m_Koromon_dead.ogg", FMOD_DEFAULT, 0, &effectSounds[static_cast<GLint>(EffectSound::M_KoromonDead)]);
	CheckResult(result);



	

	for (GLint i = 0; i < NUM_SOUND_CHEENEL; ++i)
	{
		soundChannel[i] = 0;
	}
}

GLvoid SoundManager::PlayFireSound(const EffectSound& effectSound, const GLfloat& volume)
{
	FMOD_System_PlaySound(soundSystem, effectSounds[static_cast<GLint>(effectSound)], 0, GL_FALSE, &soundChannel[static_cast<GLint>(SoundChannel::Fire)]);
	FMOD_Channel_SetVolume(soundChannel[static_cast<GLint>(SoundChannel::Fire)], volume);
}
GLvoid SoundManager::PlayWalkSound(const EffectSound& effectSound, const GLfloat& volume)
{
	FMOD_System_PlaySound(soundSystem, effectSounds[static_cast<GLint>(effectSound)], 0, GL_FALSE, &soundChannel[static_cast<GLint>(SoundChannel::Walk)]);
	FMOD_Channel_SetVolume(soundChannel[static_cast<GLint>(SoundChannel::Walk)], volume);
}

GLvoid SoundManager::PlayEffectSound(const EffectSound& effectSound, const glm::vec3& position, const GLfloat& volume)
{
	FMOD_System_Update(soundSystem);
	GLfloat v = volume;
	if (crntCamera != nullptr)
	{
		GLfloat distance = glm::length(crntCamera->GetPviotedPosition() - position);
		if (distance > EFFECT_MAX_DISTANCE)
		{
			return;
		}
		else
		{
			GLfloat t = 1 - (distance / EFFECT_MAX_DISTANCE);
			v *= t;
		}
	}

	PlayEffectSound(effectSound, v, GL_TRUE);
}
GLvoid SoundManager::PlayEffectSound(const EffectSound& effectSound, const GLfloat& volume, GLboolean isNewChannel)
{
	if (isNewChannel == GL_TRUE)
	{
		FMOD_System_PlaySound(soundSystem, effectSounds[static_cast<GLint>(effectSound)], 0, GL_FALSE, &soundChannel[static_cast<GLint>(SoundChannel::Effect)]);
		FMOD_Channel_SetVolume(soundChannel[static_cast<GLint>(SoundChannel::Effect)], volume);
	}
	else
	{
		FMOD_System_PlaySound(soundSystem, effectSounds[static_cast<GLint>(effectSound)], 0, GL_FALSE, NULL);
	}
}

GLvoid SoundManager::StopEffectSound()
{
	FMOD_Channel_Stop(soundChannel[static_cast<GLint>(SoundChannel::Effect)]);
}

GLvoid SoundManager::PlayBGMSound(const BGMSound& bgmSound, const GLfloat& volume, GLboolean isNewChannel)
{
	FMOD_System_Update(soundSystem);
	if (isNewChannel == GL_TRUE)
	{
		FMOD_System_PlaySound(soundSystem, bgmSounds[static_cast<GLint>(bgmSound)], 0, GL_FALSE, &soundChannel[static_cast<GLint>(SoundChannel::Bgm)]);
		FMOD_Channel_SetVolume(soundChannel[static_cast<GLint>(SoundChannel::Bgm)], volume);
	}
}
GLvoid SoundManager::StopBGMSound()
{
	FMOD_Channel_Stop(soundChannel[static_cast<GLint>(SoundChannel::Bgm)]);
}