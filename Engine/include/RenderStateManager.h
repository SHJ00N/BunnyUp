#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <unordered_map>
#include <string>

#include "Singleton.h"

namespace Engine
{
	struct RenderState
	{
		Microsoft::WRL::ComPtr<ID3D11BlendState> blend;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> raster;
	};

	class RenderStateManager : public Singleton<RenderStateManager>
	{
	public:
		RenderStateManager();
		~RenderStateManager();

		// create render state presets
		HRESULT Initialize();
		const RenderState& GetState(const std::string& name);
	private:
		std::unordered_map<std::string, RenderState> m_states;
	};
}