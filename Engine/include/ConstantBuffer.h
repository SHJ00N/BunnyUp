#pragma once

#include "MathHelper.h"

namespace Engine
{
	constexpr int SkinMeshBoneMax = 256;

	enum CbSlot
	{
		PerCamera = 0,
		PerObject = 1,
		SkinPerObject
	};

	struct ConstantBufferPerCamera
	{
		Matrix4x4 view;
		Matrix4x4 projection;
	};

	struct ConstantBufferPerObject
	{
		Matrix4x4 world;
	};

	struct ConstantBufferSkinPerObject
	{
		Matrix4x4 bones[SkinMeshBoneMax];
	};
}