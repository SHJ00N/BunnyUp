#include "TimeClass.h"

namespace Engine
{
	LARGE_INTEGER TimeClass::s_frequency;
	LARGE_INTEGER TimeClass::s_prevTime;

	float TimeClass::s_unscaledDeltaTime = 0.0f;
	float TimeClass::s_totalTime = 0.0f;
	float TimeClass::s_timeScale = 1.0f;
	
	float TimeClass::s_fixedDeltaTime = 0.01667f; // 60 FPS
	float TimeClass::s_accumulatedTime = 0.0f;

	int TimeClass::s_frameCount = 0;

	void TimeClass::Start()
	{
		QueryPerformanceFrequency(&s_frequency);
		QueryPerformanceCounter(&s_prevTime);
	}

	void TimeClass::Update()
	{
		// get current time
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		
		// calculate delta time
		s_unscaledDeltaTime = static_cast<float>((currentTime.QuadPart - s_prevTime.QuadPart) / static_cast<double>(s_frequency.QuadPart));
		s_prevTime = currentTime;
		if (s_unscaledDeltaTime > 0.1f)	// Clamp delta time to prevent large jumps
		{
			s_unscaledDeltaTime = 0.1f;
		}
		float scaledDeltaTime = s_unscaledDeltaTime * s_timeScale;

		s_totalTime += scaledDeltaTime;
		s_accumulatedTime += scaledDeltaTime;
		if(s_accumulatedTime > 0.2f)	// Clamp accumulated time to prevent spiral of death
		{
			s_accumulatedTime = 0.2f;
		}
		
		// frame management
		s_frameCount++;
	}
}