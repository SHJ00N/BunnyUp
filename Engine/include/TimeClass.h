#pragma once

#include <Windows.h>

namespace Engine
{
	class TimeClass
	{
	public:
		// Control the flow of time in the application
		static void Start();
		static void Update();

		// setter
		static void SetTimeScale(float timeScale) { s_timeScale = timeScale; }

		// getter
		static float GetDeltaTime() { return s_unscaledDeltaTime * s_timeScale; }
		static float GetUnscaledDeltaTime() { return s_unscaledDeltaTime; }
		static float GetTotalTime() { return s_totalTime; }
		static float GetTimeScale() { return s_timeScale; }
		static float GetFixedDeltaTime() { return s_fixedDeltaTime; }

		// fixed update management
		static bool ShouldPerformFixedUpdate() { return s_accumulatedTime >= s_fixedDeltaTime; }
		static void ConsumeFixedUpdateTime() { s_accumulatedTime -= s_fixedDeltaTime; }

	private:
		// Calculate the delta time and update the previous time point.
		static LARGE_INTEGER s_frequency;
		static LARGE_INTEGER s_prevTime;
		static float s_unscaledDeltaTime;
		static float s_totalTime;
		
		static float s_fixedDeltaTime;
		static float s_accumulatedTime;

		// Time scale is a multiplier for delta time, allowing for effects like slow motion or fast forward.
		static float s_timeScale;

		static int s_frameCount;
	};
}