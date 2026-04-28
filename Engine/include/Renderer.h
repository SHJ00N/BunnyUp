#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "ConstantBuffer.h"
#include "D3DManager.h"

namespace Engine
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();
		HRESULT Initialize();

		template <typename T>
		void UpdateConstantBuffer(ID3D11Buffer* buffer, UINT slot, const T& data)
		{
			ID3D11DeviceContext* context = D3DManager::GetInstance().GetDeviceContext();
			// Update constant buffer
			context->UpdateSubresource(buffer, 0, nullptr, &data, 0, 0);
			context->VSSetConstantBuffers(slot, 1, &buffer);
		}
		void UpdatePerCamera(const ConstantBufferPerCamera& data);
		void UpdatePerObject(const ConstantBufferPerObject& data);
		void UpdateSkinPerObject(const ConstantBufferSkinPerObject& data);
		
	private:
		// constant buffers
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBufferPerCamera;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBufferPerObject;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBufferSkinPerObject;
	};
}