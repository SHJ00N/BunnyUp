#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d11.h>
#include <wrl/client.h>

namespace Engine
{
	class D3DClass
	{
	public:
		D3DClass();
		~D3DClass();

		HRESULT CreateDeviceResources();
		HRESULT CreateWindowResources(HWND hWnd);

		HRESULT ConfigureBackBuffer();
		HRESULT ReleaseBackBuffer();
		HRESULT GoFullScreen();
		HRESULT GoWindowed();

		void BeginFrame(float r, float g, float b, float a);
		void EndFrame();

	private:
		// direct3D device
		Microsoft::WRL::ComPtr<ID3D11Device> m_pd3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pd3dDeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_pDXGISwapChain;

		// DXGI swap chain device resources for back buffer
		Microsoft::WRL::ComPtr < ID3D11Texture2D> m_pBackBuffer;
		Microsoft::WRL::ComPtr < ID3D11RenderTargetView> m_pRenderTarget;

		// DXGI swap chain device resources for depth stencil buffer
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_pDepthStencilView;

		// meta data
		D3D_FEATURE_LEVEL m_featureLevel;
		D3D11_TEXTURE2D_DESC m_backBufferDesc;
		D3D11_VIEWPORT m_viewport;
	};
}