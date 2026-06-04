#pragma once

#include <d3d11.h>
#include <memory>
#include <string>
#include "MathHelper.h"

namespace Engine
{
	class Texture2D;
	class RenderTargetResource;

	inline constexpr unsigned int EnvCubeMapSize = 1024;
	inline constexpr unsigned int IrradianceMapSize = 32;
	inline constexpr unsigned int PrefilteredEnvMapSize = 128;
	inline unsigned int BrdfLUTSize = 512;
	inline constexpr unsigned int MipLevels = 5;

	class EnvironmentMap
	{
	public:
		HRESULT CreateFromHDRFile(const std::string& filePath);

		RenderTargetResource* GetEnvCubeMap() const { return m_pEnvCubeMap.get(); }
		RenderTargetResource* GetIrradianceMap() const { return m_pIrradianceMap.get(); }
		RenderTargetResource* GetPrefilteredEnvMap() const { return m_pPrefilteredEnvMap.get(); }
		RenderTargetResource* GetBrdfLUT() const { return m_pBrdfLUT.get(); }

	private:
		std::shared_ptr<Texture2D> m_pHDRTexture;
		std::shared_ptr<RenderTargetResource> m_pEnvCubeMap;
		std::shared_ptr<RenderTargetResource> m_pIrradianceMap;
		std::shared_ptr<RenderTargetResource> m_pPrefilteredEnvMap;
		std::shared_ptr<RenderTargetResource> m_pBrdfLUT;

		HRESULT generateCubemap(ID3D11Device* device, ID3D11DeviceContext* context, const Matrix4x4& captureProjection, const Matrix4x4* captureViews);
		HRESULT generateIrradianceMap(ID3D11Device* device, ID3D11DeviceContext* context, const Matrix4x4& captureProjection, const Matrix4x4* captureViews);
		HRESULT generatePrefilteredEnvMap(ID3D11Device* device, ID3D11DeviceContext* context, const Matrix4x4& captureProjection, const Matrix4x4* captureViews);
		HRESULT generateBrdfLUT(ID3D11Device* device, ID3D11DeviceContext* context);
	};
}
