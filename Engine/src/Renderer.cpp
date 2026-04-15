#include "Renderer.h"
#include "MathHelper.h"
#include "D3DManager.h"
#include "Mesh.h"
#include "Material.h"
#include "RenderStateManager.h"
#include "Model.h"
#include "ResourceManager.h"

#include <cmath>

namespace Engine
{
	Renderer::Renderer() : m_pConstantBuffer(nullptr)
	{
	}
  
	Renderer::~Renderer()
	{
	}

    HRESULT Renderer::Initialize()
    {
        // create cube
        HRESULT hr = S_OK;

		ResourceManager::GetInstance().LoadShader<VertexPT>("Default", "C:\\Project\\BunnyUp\\Engine\\TextureShader.hlsl");
		ResourceManager::GetInstance().LoadModel("Chibi_Rabbit", "C:\\Project\\BunnyUp\\Engine\\resources\\models\\Chibi_Rabbit.fbx");

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

        // model.LoadModel("C:\\Project\\BunnyUp\\Engine\\Chibi_Rabbit.fbx");

        // Create cube geometry.
        std::vector<VertexPT> CubeVertices =
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
		m_constantBufferData.view = Transpose(view);

        float aspectRatioX = D3DManager::GetInstance().GetAspectRatio();
        float aspectRatioY = aspectRatioX < (16.0f / 9.0f) ? aspectRatioX / (16.0f / 9.0f) : 1.0f;

		Matrix4x4 projection = PerspectiveFovLH(2.0f * std::atan(std::tan(Radians(70.0f) * 0.5f) / aspectRatioY), aspectRatioX, 0.01f, 100.0f);
		m_constantBufferData.projection = Transpose(projection);

		return hr;
	}

	void Renderer::Update()
	{
        if (AutoRotate)
        {
            Rotation += RotationSpeed;
            if(Rotation >= 360.0f)
            {
                Rotation = 0.0f;
			}
        }
		Quaternion q = AngleAxis(Rotation, Vector3(0, 1.0f, 0));
        Matrix4x4 transform = TransformMatrix(Vector3(0, 0, 0), q, Vector3(Scale, Scale, Scale));
        m_constantBufferData.world = Transpose(transform);
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

        context->VSSetConstantBuffers(
            0,
            1,
            m_pConstantBuffer.GetAddressOf()
        );

		ResourceManager::GetInstance().GetModel("Chibi_Rabbit")->Render();
	}
}