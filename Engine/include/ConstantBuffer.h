#pragma once

#include "MathHelper.h"

namespace Engine
{
	typedef struct ConstantBuffer
	{
		Matrix4x4 world;
		Matrix4x4 view;
		Matrix4x4 projection;
	} ConstantBufferStruct;
}