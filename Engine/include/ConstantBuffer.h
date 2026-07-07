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
		PerLight = 4,
		PrefilteredEnvMap = 5,
	};

	struct ConstantBufferPerCamera
	{
		Vector4 position;
		Matrix4x4 view;
		Matrix4x4 projection;
		Matrix4x4 invView;
		Matrix4x4 invProjection;
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
		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		float roughness = 1.0f;
		float metallic = 0.0f;
		uint32_t hasNormal = false;
		uint32_t hasRoughness = false;
		uint32_t hasMetallic = false;
		uint32_t hasAmbientOcclusion = false;
		Vector2 padding;
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
		uint32_t lightCount = 0;
		uint32_t padding[3];
	};

	// IBL prefiltered environment map
	struct ConstantBufferPrefilteredEnvMap
	{
		float roughness;
		float resolution;
		unsigned int maxMip;
		float padding; // Padding to align to 16 bytes
	};
}