#include "Renderer.h"
#include "MathHelper.h"
#include "D3DManager.h"
#include "ResourceManager.h"

#include <cmath>

namespace Engine
{
	Renderer::Renderer()
	{
	}
  
	Renderer::~Renderer()
	{
	}

    HRESULT Renderer::Initialize()
    {
        // create cube
        HRESULT hr = S_OK;

        // Use the Direct3D device to load resources into graphics memory.
        ID3D11Device* device = D3DManager::GetInstance().GetDevice();

        // Create Constant buffers
        CD3D11_BUFFER_DESC cbPerCameraDesc(sizeof(ConstantBufferPerCamera), D3D11_BIND_CONSTANT_BUFFER);
        hr = device->CreateBuffer(&cbPerCameraDesc, nullptr, m_pConstantBufferPerCamera.GetAddressOf());
        if (FAILED(hr)) return hr;

        CD3D11_BUFFER_DESC cbPerObjectDesc(sizeof(ConstantBufferPerObject), D3D11_BIND_CONSTANT_BUFFER);
        hr = device->CreateBuffer(&cbPerObjectDesc, nullptr, m_pConstantBufferPerObject.GetAddressOf());
        if (FAILED(hr)) return hr;

        CD3D11_BUFFER_DESC cbSkinPerObjectDesc(sizeof(ConstantBufferSkinPerObject), D3D11_BIND_CONSTANT_BUFFER);
        hr = device->CreateBuffer(&cbSkinPerObjectDesc, nullptr, m_pConstantBufferSkinPerObject.GetAddressOf());
        if (FAILED(hr)) return hr;

        CD3D11_BUFFER_DESC cbPerMaterialDesc(sizeof(ConstantBufferPerMaterial), D3D11_BIND_CONSTANT_BUFFER);
        hr = device->CreateBuffer(&cbPerMaterialDesc, nullptr, m_pConstantBufferPerMaterial.GetAddressOf());
        if (FAILED(hr)) return hr;
        
		return hr;
	}

    void Renderer::UpdatePerCamera(const ConstantBufferPerCamera& data)
    {
        UpdateConstantBuffer(m_pConstantBufferPerCamera.Get(), CbSlot::PerCamera, data);
    }

    void Renderer::UpdatePerObject(const ConstantBufferPerObject& data)
    {
        UpdateConstantBuffer(m_pConstantBufferPerObject.Get(), CbSlot::PerObject, data);
    }

    void Renderer::UpdateSkinPerObject(const ConstantBufferSkinPerObject& data)
    {
        UpdateConstantBuffer(m_pConstantBufferSkinPerObject.Get(), CbSlot::SkinPerObject, data);
    }

    void Renderer::UpdatePerMaterial(const ConstantBufferPerMaterial& data)
    {
        UpdateConstantBuffer(m_pConstantBufferPerMaterial.Get(), CbSlot::PerMaterial, data);
    }
}