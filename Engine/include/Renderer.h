#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

#include "ConstantBuffer.h"
#include "Transform.h"

namespace Engine
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();
		HRESULT Initialize();
		void Update();

		void SetWorldMatrix(const Matrix4x4& world) { m_constantBufferData.world = world; }
		void UpdateConstantBuffer();


		// Temporary code for debugging
		float Scale = 0.09f;
		float Rotation = 0.0f;
		bool AutoRotate = false;
		float RotationSpeed = 1.0f;
		Transform transform;
	private:
		ConstantBufferStruct m_constantBufferData;
		Microsoft::WRL::ComPtr<ID3D11Buffer>            m_pConstantBuffer;
	};
}