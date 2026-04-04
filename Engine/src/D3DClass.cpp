#include "D3DClass.h"

#include <dxgi1_3.h>

namespace Engine
{
	D3DClass::D3DClass() : m_featureLevel(D3D_FEATURE_LEVEL_11_0), m_backBufferDesc(0), m_viewport(0)
	{
	}

	D3DClass::~D3DClass()
	{
	}

	HRESULT D3DClass::CreateDeviceResources()
	{
		D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_9_1,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1
		};

		UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		// Create the device and device context
		Microsoft::WRL::ComPtr<ID3D11Device>        device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

		if (FAILED(D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			deviceFlags,
			levels,
			ARRAYSIZE(levels),
			D3D11_SDK_VERSION,
			&device,
			&m_featureLevel,
			&context)))
		{
			DWORD dwError = GetLastError();
			return HRESULT_FROM_WIN32(dwError);
		}

		// Store the device and device context
		device.As(&m_pd3dDevice);
		context.As(&m_pd3dDeviceContext);

		return S_OK;
	}

	HRESULT D3DClass::CreateWindowResources(HWND hWnd)
	{
		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
		desc.Windowed = TRUE; // Sets the initial state of full-screen mode.
		desc.BufferCount = 2;
		desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;      //multisampling setting
		desc.SampleDesc.Quality = 0;    //vendor-specific flag
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		desc.OutputWindow = hWnd;

		Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
		m_pd3dDevice.As(&dxgiDevice);

		// Create swap chain
		Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
		Microsoft::WRL::ComPtr<IDXGIFactory> factory;

		// Get the adapter and factory from the device
		if (SUCCEEDED(dxgiDevice->GetAdapter(&adapter)))
		{
			adapter->GetParent(IID_PPV_ARGS(&factory));

			factory->CreateSwapChain(m_pd3dDevice.Get(), &desc, &m_pDXGISwapChain);
		}
		else
		{
			DWORD dwError = GetLastError();
			return HRESULT_FROM_WIN32(dwError);
		}

		// Configure the back buffer
		if(FAILED(ConfigureBackBuffer()))
		{
			DWORD dwError = GetLastError();
			return HRESULT_FROM_WIN32(dwError);
		}

		return S_OK;
	}

	HRESULT D3DClass::ConfigureBackBuffer()
	{
		// Get the back buffer from the swap chain
		if (FAILED(m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_pBackBuffer)))
		{
			DWORD dwError = GetLastError();
			return HRESULT_FROM_WIN32(dwError);
		}

		// Create a render target view for the back buffer
		if (FAILED(m_pd3dDevice->CreateRenderTargetView(m_pBackBuffer.Get(), nullptr, m_pRenderTarget.GetAddressOf())))
		{
			DWORD dwError = GetLastError();
			return HRESULT_FROM_WIN32(dwError);
		}

		m_pBackBuffer->GetDesc(&m_backBufferDesc);

		// Create a depth-stencil view
		CD3D11_TEXTURE2D_DESC depthStencilDesc(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			static_cast<UINT> (m_backBufferDesc.Width),
			static_cast<UINT> (m_backBufferDesc.Height),
			1, // This depth stencil view has only one texture.
			1, // Use a single mipmap level.
			D3D11_BIND_DEPTH_STENCIL
		);

		m_pd3dDevice->CreateTexture2D(
			&depthStencilDesc,
			nullptr,
			&m_pDepthStencilBuffer
		);

		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

		m_pd3dDevice->CreateDepthStencilView(
			m_pDepthStencilBuffer.Get(),
			&depthStencilViewDesc,
			&m_pDepthStencilView
		);

		// Set viewport
		ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
		m_viewport.Height = (float)m_backBufferDesc.Height;
		m_viewport.Width = (float)m_backBufferDesc.Width;
		m_viewport.MinDepth = 0;
		m_viewport.MaxDepth = 1;

		m_pd3dDeviceContext->RSSetViewports(1, &m_viewport);

		return S_OK;
	}

	HRESULT D3DClass::ReleaseBackBuffer()
	{
		// Release the render target view based on the back buffer
		m_pRenderTarget.Reset();

		// Release the back buffer
		m_pBackBuffer.Reset();

		// The depth stencil will need to be resized, so release it (and view):
		m_pDepthStencilView.Reset();
		m_pDepthStencilBuffer.Reset();

		// Flush the device context to ensure that the references to the back buffer and depth stencil are released before resizing
		m_pd3dDeviceContext->Flush();

		return S_OK;
	}

	HRESULT D3DClass::GoFullScreen()
	{
		HRESULT hr = S_OK;

		hr = m_pDXGISwapChain->SetFullscreenState(TRUE, NULL);

		// Before calling ResizeBuffers, you have to release all references to the back 
		// buffer device resource.
		ReleaseBackBuffer();

		// Now we can call ResizeBuffers.
		hr = m_pDXGISwapChain->ResizeBuffers(
			0,                   // Number of buffers. Set this to 0 to preserve the existing setting.
			0, 0,                // Width and height of the swap chain. Set to 0 to match the screen resolution.
			DXGI_FORMAT_UNKNOWN, // This tells DXGI to retain the current back buffer format.
			0
		);

		// Then we can recreate the back buffer, depth buffer, and so on.
		hr = ConfigureBackBuffer();

		return hr;
	}

	HRESULT D3DClass::GoWindowed()
	{
		HRESULT hr = S_OK;

		hr = m_pDXGISwapChain->SetFullscreenState(FALSE, NULL);

		// Before calling ResizeBuffers, you have to release all references to the back 
		// buffer device resource.
		ReleaseBackBuffer();

		// Now we can call ResizeBuffers.
		hr = m_pDXGISwapChain->ResizeBuffers(
			0,                   // Number of buffers. Set this to 0 to preserve the existing setting.
			0, 0,                // Width and height of the swap chain. MUST be set to a non-zero value. For example, match the window size.
			DXGI_FORMAT_UNKNOWN, // This tells DXGI to retain the current back buffer format.
			0
		);

		// Then we can recreate the back buffer, depth buffer, and so on.
		hr = ConfigureBackBuffer();

		return hr;
	}

	void D3DClass::Present()
	{
		m_pDXGISwapChain->Present(1, 0);
	}
}

