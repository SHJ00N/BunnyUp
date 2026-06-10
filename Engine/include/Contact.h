#pragma once

#include "MathHelper.h"

namespace Engine
{
	class Collider;
	struct Contact
	{
		Collider* a = nullptr;
		Collider* b = nullptr;

		Vector3 normal;
		float penetration;
	};
}