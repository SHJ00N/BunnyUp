#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <unordered_map>
#include <string>

#include "Singleton.h"

namespace Engine
{
	enum class BlendType
	{
		Disable,
		Alpha
	};
	enum class DepthType
	{
		Disable,
		ReadWrite,
		Read
	};
	enum class RasterType
	{
		None,
		Front,
		Back
	};

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
		ID3D11BlendState* GetBlendState(BlendType type);
		ID3D11DepthStencilState* GetDepthState(DepthType type);
		ID3D11RasterizerState* GetRasterState(RasterType type);
	private:
		std::unordered_map<std::string, RenderState> m_states;		// preset
		std::unordered_map<BlendType, Microsoft::WRL::ComPtr<ID3D11BlendState>> m_blendStates;
		std::unordered_map<DepthType, Microsoft::WRL::ComPtr<ID3D11DepthStencilState>> m_depthStates;
		std::unordered_map<RasterType, Microsoft::WRL::ComPtr<ID3D11RasterizerState>> m_rasterStates;
	};
}