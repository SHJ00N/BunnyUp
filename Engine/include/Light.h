#pragma once

#include "Component.h"
#include "MathHelper.h"

namespace Engine
{
	enum LightType
	{
		Directional,
		Point,
		Spot
	};

	class Light : public Component
	{
	public:
		// Common light properties
		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);	// rgb + intensity
		LightType type = LightType::Point;
		bool isEnabled = true;
		// For spot light
		float innerCutOff = 20.0f;
		float outerCutOff = 30.0f;

	protected:
		void OnAwake() override;
		void OnDestroy() override;
	};
}