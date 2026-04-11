#include "Renderer.h"
#include "MathHelper.h"
#include "D3DManager.h"

#include <cmath>

namespace Engine
{
	Renderer::Renderer() : m_pVertexBuffer(nullptr), m_pIndexBuffer(nullptr), m_pConstantBuffer(nullptr), m_indexCount(0), m_frameCount(0)
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

        CD3D11_BUFFER_DESC cbDesc(
            sizeof(ConstantBufferStruct),
            D3D11_BIND_CONSTANT_BUFFER
        );

        hr = device->CreateBuffer(
            &cbDesc,
            nullptr,
            m_pConstantBuffer.GetAddressOf()
        );

        // Create cube geometry.
        //VertexPositionColor CubeVertices[] =
        //{
        //    {DirectX::XMFLOAT3(-0.5f,-0.5f,-0.5f), DirectX::XMFLOAT3(0,   0,   0),},
        //    {DirectX::XMFLOAT3(-0.5f,-0.5f, 0.5f), DirectX::XMFLOAT3(0,   0,   1),},
        //    {DirectX::XMFLOAT3(-0.5f, 0.5f,-0.5f), DirectX::XMFLOAT3(0,   1,   0),},
        //    {DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(0,   1,   1),},

        //    {DirectX::XMFLOAT3(0.5f,-0.5f,-0.5f), DirectX::XMFLOAT3(1,   0,   0),},
        //    {DirectX::XMFLOAT3(0.5f,-0.5f, 0.5f), DirectX::XMFLOAT3(1,   0,   1),},
        //    {DirectX::XMFLOAT3(0.5f, 0.5f,-0.5f), DirectX::XMFLOAT3(1,   1,   0),},
        //    {DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(1,   1,   1),},
        //};

        VertexPositionUV CubeVertices[] =
        {
            // -X
            {DirectX::XMFLOAT3{-0.5f,-0.5f,-0.5f}, DirectX::XMFLOAT2{0,1}},
            {DirectX::XMFLOAT3{-0.5f,-0.5f, 0.5f}, DirectX::XMFLOAT2{1,1}},
            {DirectX::XMFLOAT3{-0.5f, 0.5f,-0.5f}, DirectX::XMFLOAT2{0,0}},
            {DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{1,0}},

            // +X
            {DirectX::XMFLOAT3{0.5f,-0.5f,-0.5f}, DirectX::XMFLOAT2{1,1}},
            {DirectX::XMFLOAT3{0.5f,-0.5f, 0.5f}, DirectX::XMFLOAT2{0,1}},
            {DirectX::XMFLOAT3{0.5f, 0.5f,-0.5f}, DirectX::XMFLOAT2{1,0}},
            {DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0,0}},

            // -Y
            {DirectX::XMFLOAT3{-0.5f,-0.5f,-0.5f}, DirectX::XMFLOAT2{0,1}},
            {DirectX::XMFLOAT3{-0.5f,-0.5f, 0.5f}, DirectX::XMFLOAT2{0,0}},
            {DirectX::XMFLOAT3{ 0.5f,-0.5f,-0.5f}, DirectX::XMFLOAT2{1,1}},
            {DirectX::XMFLOAT3{ 0.5f,-0.5f, 0.5f}, DirectX::XMFLOAT2{1,0}},

            // +Y
            {DirectX::XMFLOAT3{-0.5f,0.5f,-0.5f}, DirectX::XMFLOAT2{0,0}},
            {DirectX::XMFLOAT3{-0.5f,0.5f, 0.5f}, DirectX::XMFLOAT2{0,1}},
            {DirectX::XMFLOAT3{ 0.5f,0.5f,-0.5f}, DirectX::XMFLOAT2{1,0}},
            {DirectX::XMFLOAT3{ 0.5f,0.5f, 0.5f}, DirectX::XMFLOAT2{1,1}},

            // -Z
            {DirectX::XMFLOAT3{-0.5f,-0.5f,-0.5f}, DirectX::XMFLOAT2{1,1}},
            {DirectX::XMFLOAT3{-0.5f, 0.5f,-0.5f}, DirectX::XMFLOAT2{1,0}},
            {DirectX::XMFLOAT3{ 0.5f,-0.5f,-0.5f}, DirectX::XMFLOAT2{0,1}},
            {DirectX::XMFLOAT3{ 0.5f, 0.5f,-0.5f}, DirectX::XMFLOAT2{0,0}},

            // +Z
            {DirectX::XMFLOAT3{-0.5f,-0.5f,0.5f}, DirectX::XMFLOAT2{0,1}},
            {DirectX::XMFLOAT3{-0.5f, 0.5f,0.5f}, DirectX::XMFLOAT2{0,0}},
            {DirectX::XMFLOAT3{ 0.5f,-0.5f,0.5f}, DirectX::XMFLOAT2{1,1}},
            {DirectX::XMFLOAT3{ 0.5f, 0.5f,0.5f}, DirectX::XMFLOAT2{1,0}},
        };


        // Create vertex buffer:
        CD3D11_BUFFER_DESC vDesc(
            sizeof(CubeVertices),
            D3D11_BIND_VERTEX_BUFFER
        );

        D3D11_SUBRESOURCE_DATA vData;
        ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
        vData.pSysMem = CubeVertices;
        vData.SysMemPitch = 0;
        vData.SysMemSlicePitch = 0;

        hr = device->CreateBuffer(
            &vDesc,
            &vData,
            &m_pVertexBuffer
        );
        if (FAILED(hr)) return hr;

        // Create index buffer:
        unsigned short CubeIndices[] =
        {
            //0,2,1, // -x
            //1,2,3,

            //4,5,6, // +x
            //5,7,6,

            //0,1,5, // -y
            //0,5,4,

            //2,6,7, // +y
            //2,7,3,

            //0,4,6, // -z
            //0,6,2,

            //1,3,7, // +z
            //1,7,5,
            0,2,1, 1,2,3,       // -X
            4,5,6, 5,7,6,       // +X
            8,10,9, 9,10,11,    // -Y
            12,13,14, 13,15,14, // +Y
            16,17,18, 17,19,18, // -Z
            20,22,21, 21,22,23  // +Z
        };

        m_indexCount = ARRAYSIZE(CubeIndices);

        CD3D11_BUFFER_DESC iDesc(
            sizeof(CubeIndices),
            D3D11_BIND_INDEX_BUFFER
        );

        D3D11_SUBRESOURCE_DATA iData;
        ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
        iData.pSysMem = CubeIndices;
        iData.SysMemPitch = 0;
        iData.SysMemSlicePitch = 0;

        hr = device->CreateBuffer(
            &iDesc,
            &iData,
            &m_pIndexBuffer
        );
        if (FAILED(hr)) return hr;

        // view and projection
		Matrix4x4 view = LookAtLH(Vector3(0, 0.7f, 1.5f), Vector3(0, -0.1f, 0), Vector3(0, 1.0f, 0));
		m_constantBufferData.view = Transpose(view);

        float aspectRatioX = D3DManager::GetInstance().GetAspectRatio();
        float aspectRatioY = aspectRatioX < (16.0f / 9.0f) ? aspectRatioX / (16.0f / 9.0f) : 1.0f;

		Matrix4x4 projection = PerspectiveFovLH(2.0f * std::atan(std::tan(Radians(70.0f) * 0.5f) / aspectRatioY), aspectRatioX, 0.01f, 100.0f);
		m_constantBufferData.projection = Transpose(projection);

        hr = m_shader.CompileFromFile<VertexPT>(L"C:\\Project\\BunnyUp\\Engine\\TextureShader.hlsl");
        if (FAILED(hr))
        {
            return hr;
        }

        hr = m_texture.CreateFromFile(L"C:\\Project\\BunnyUp\\Engine\\container.jpg");
        if (FAILED(hr))
        {
            return hr;
        }

        hr = m_sampler.CreateSampler(SamplerType::LinearClamp);

		return hr;
	}

	void Renderer::Update()
	{

        Quaternion q = AngleAxis((float)m_frameCount++, Vector3(0, 1, 0));
        Matrix4x4 rot = Rotation(q);
        m_constantBufferData.world = Transpose(rot);

        if (m_frameCount == MAXUINT)  m_frameCount = 0;
	}

	void Renderer::Render()
	{
        ID3D11DeviceContext* context = D3DManager::GetInstance().GetDeviceContext();

        ID3D11RenderTargetView* renderTarget = D3DManager::GetInstance().GetRenderTarget();
        ID3D11DepthStencilView* depthStencil = D3DManager::GetInstance().GetDepthStencil();

        context->UpdateSubresource(
            m_pConstantBuffer.Get(),
            0,
            nullptr,
            &m_constantBufferData,
            0,
            0
        );

        UINT stride = sizeof(VertexPositionUV);
        UINT offset = 0;

        context->IASetVertexBuffers(
            0,
            1,
            m_pVertexBuffer.GetAddressOf(),
            &stride,
            &offset
        );

        context->IASetIndexBuffer(
            m_pIndexBuffer.Get(),
            DXGI_FORMAT_R16_UINT,
            0
        );

        context->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        );

        m_shader.Bind();

        m_texture.Bind(0);
        m_sampler.Bind(0);

        context->VSSetConstantBuffers(
            0,
            1,
            m_pConstantBuffer.GetAddressOf()
        );

        context->DrawIndexed(
            m_indexCount,
            0,
            0
        );
	}
}