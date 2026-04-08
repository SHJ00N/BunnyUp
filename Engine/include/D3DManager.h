#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <wrl/client.h>

#include "Singleton.h"

namespace Engine
{
	class D3DManager : public Singleton<D3DManager>
	{
	public:
		D3DManager();
		~D3DManager();

		HRESULT CreateDeviceResources();
		HRESULT CreateWindowResources(HWND hWnd);

		HRESULT ConfigureBackBuffer();
		HRESULT ReleaseBackBuffer();
		HRESULT GoFullScreen();
		HRESULT GoWindowed();

		void BeginFrame(float r, float g, float b, float a);
		void EndFrame();

		float GetAspectRatio();

		ID3D11Device* GetDevice() { return m_pd3dDevice.Get(); };
		ID3D11DeviceContext* GetDeviceContext() { return m_pd3dDeviceContext.Get(); };
		ID3D11RenderTargetView* GetRenderTarget() { return m_pRenderTarget.Get(); }
		ID3D11DepthStencilView* GetDepthStencil() { return m_pDepthStencilView.Get(); }

	private:
		// direct3D device
		Microsoft::WRL::ComPtr<ID3D11Device> m_pd3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pd3dDeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_pDXGISwapChain;

		// DXGI swap chain device resources for back buffer
		Microsoft::WRL::ComPtr <ID3D11Texture2D> m_pBackBuffer;
		Microsoft::WRL::ComPtr <ID3D11RenderTargetView> m_pRenderTarget;

		// DXGI swap chain device resources for depth stencil buffer
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_pDepthStencilView;

		// meta data
		D3D_FEATURE_LEVEL m_featureLevel;
		D3D11_TEXTURE2D_DESC m_backBufferDesc;
		D3D11_VIEWPORT m_viewport;
	};
}