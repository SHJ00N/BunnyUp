#pragma once

#include "MathHelper.h"

namespace Engine
{
	constexpr int SkinMeshBoneMax = 256;

	enum CbSlot
	{
		PerCamera = 0,
		PerObject = 1,
		SkinPerObject = 2,
		PerMaterial = 3,
		PerLight = 4
	};

	struct ConstantBufferPerCamera
	{
		Vector4 position;
		Matrix4x4 view;
		Matrix4x4 projection;
	};

	struct ConstantBufferPerObject
	{
		Matrix4x4 world;
		Matrix4x4 normalMatrix;
	};

	struct ConstantBufferSkinPerObject
	{
		Matrix4x4 bones[SkinMeshBoneMax];
	};

	struct ConstantBufferPerMaterial
	{
		Vector4 color;
	};

	// Light
	struct LightBuffer
	{
		Vector4 position; // w is type (0: directional, 1: point, 2: spot)
		Vector4 direction; // w is unused
		Vector4 color; // rgb + intensity
		Vector4 parameters; // x: innerCutOff, y: outerCutOff, z: isEnabled, w: unused
	};

	static constexpr int MaxLights = 256;
	struct ConstantBufferPerLight
	{
		LightBuffer lights[MaxLights];
		uint32_t lightCount;
		uint32_t padding[3];
	};
}