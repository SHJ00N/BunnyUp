#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "ConstantBuffer.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Sampler.h"

namespace Engine
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();
		HRESULT Initialize();
		void Update();
		void Render();

		typedef struct _vertexPositionColor
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 color;
		} VertexPositionColor;

		typedef struct _vertexPositionUV
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT2 uv;
		} VertexPositionUV;

	private:
		ConstantBufferStruct m_constantBufferData;
		unsigned int  m_indexCount;
		unsigned int  m_frameCount;
		Microsoft::WRL::ComPtr<ID3D11Buffer>            m_pVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>            m_pIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>            m_pConstantBuffer;
		Shader m_shader;
		Texture2D m_texture;
		Sampler m_sampler;
	};
}