#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "ConstantBuffer.h"

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

	private:
		ConstantBufferStruct m_constantBufferData;
		Microsoft::WRL::ComPtr<ID3D11Buffer>            m_pConstantBuffer;
	};
}