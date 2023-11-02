#pragma once
#include "stdafx.h"

enum class Timer : GLint {
	LightRotation_Y,
};

namespace timer {
	/* 등록된 모든 Timer 초기화 */
	GLvoid Init();

	/* 해당 Timer 초기화 */
	GLvoid InitTimer(const Timer& timer);
	GLvoid InitTimer(GLboolean(*timerFunc)(GLint));

	/* 해당 Timer를 실행 준비 상태로 설정 */
	GLvoid ReadyTimer(const Timer& timer);

	/* 등록된 모든 Timer Update */
	GLvoid Update();
	/* Enable Timer Update */
	GLvoid StartUpdate();
	/* Disable Timer Update */
	GLvoid StopUpdate();

	/* Timer Enable/Disable */
	GLvoid ToggleTimer(const Timer& timer);
	GLvoid ToggleTimer(GLboolean(*timerFunc)(GLint));
	/* Timer Enable */
	GLvoid EnableTimer(const Timer& timer);
	GLvoid EnableTimer(GLboolean(*timerFunc)(GLint));
	/* Timer Disable */
	GLvoid DisableTimer(const Timer& timer);
	GLvoid DisableTimer(GLboolean(*timerFunc)(GLint));
	
	/* FPS를 계산하여 DeltaTime에 사용 */
	GLvoid CalculateFPS();
	/* 경과한 시간(DeltaTime) 반환 */
	GLfloat DeltaTime();

	GLvoid StartRecord();
	GLvoid PrintDuration();
}

