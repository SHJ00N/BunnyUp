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


        // Create cube geometry.
        std::vector<VertexPU> CubeVertices =
        {
            // -X
            {Vector3{-0.5f, 0.5f, 0.5f}, Vector2{1,0}},
            {Vector3{-0.5f, 0.5f,-0.5f}, Vector2{0,0}},
            {Vector3{-0.5f,-0.5f,-0.5f}, Vector2{0,1}},
            {Vector3{-0.5f,-0.5f, 0.5f}, Vector2{1,1}},

            // +X
            {Vector3{0.5f, 0.5f, 0.5f}, Vector2{0,0}},
            {Vector3{0.5f,-0.5f,-0.5f}, Vector2{1,1}},
            {Vector3{0.5f, 0.5f,-0.5f}, Vector2{1,0}},
            {Vector3{0.5f,-0.5f, 0.5f}, Vector2{0,1}},

            // -Y
            {Vector3{-0.5f,-0.5f,-0.5f}, Vector2{0,1}},
            {Vector3{ 0.5f,-0.5f,-0.5f}, Vector2{1,1}},
            {Vector3{ 0.5f,-0.5f, 0.5f}, Vector2{1,0}},
            {Vector3{-0.5f,-0.5f, 0.5f}, Vector2{0,0}},

            // +Y
            {Vector3{-0.5f,0.5f,-0.5f}, Vector2{0,0}},
            {Vector3{ 0.5f,0.5f, 0.5f}, Vector2{1,1}},
            {Vector3{ 0.5f,0.5f,-0.5f}, Vector2{1,0}},
            {Vector3{-0.5f,0.5f, 0.5f}, Vector2{0,1}},

            // -Z
            {Vector3{-0.5f,-0.5f,-0.5f}, Vector2{1,1}},
            {Vector3{ 0.5f, 0.5f,-0.5f}, Vector2{0,0}},
            {Vector3{ 0.5f,-0.5f,-0.5f}, Vector2{0,1}},
            {Vector3{-0.5f, 0.5f,-0.5f}, Vector2{1,0}},

            // +Z
            {Vector3{-0.5f,-0.5f,0.5f}, Vector2{0,1}},
            {Vector3{ 0.5f,-0.5f,0.5f}, Vector2{1,1}},
            {Vector3{ 0.5f, 0.5f,0.5f}, Vector2{1,0}},
            {Vector3{-0.5f, 0.5f,0.5f}, Vector2{0,0}},
        };

        // Create index buffer:
        std::vector<uint32_t> CubeIndices =
        {
            // -X
            0,1,2, 2,3,0,

            // +X
            4,5,6, 5,4,7,

            // -Y
            8,9,10, 10,11,8,

            // +Y
            12,13,14, 13,12,15,

            // -Z
            16,17,18, 17,16,19,

            // +Z
            20,21,22, 22,23,20
        };

        // view and projection
		Matrix4x4 view = LookAtLH(Vector3(0, 10.0f, 20.5f), Vector3(0, 10.0f, 0), Vector3(0, 1.0f, 0));
        m_cbPerCamera.view = view;

        float aspectRatioX = D3DManager::GetInstance().GetAspectRatio();
        float aspectRatioY = aspectRatioX < (16.0f / 9.0f) ? aspectRatioX / (16.0f / 9.0f) : 1.0f;

		Matrix4x4 projection = PerspectiveFovLH(2.0f * std::atan(std::tan(Radians(70.0f) * 0.5f) / aspectRatioY), aspectRatioX, 0.01f, 100.0f);
        m_cbPerCamera.projection = projection;

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

    void Renderer::Update()
    {
        UpdatePerCamera(m_cbPerCamera);
    }
}