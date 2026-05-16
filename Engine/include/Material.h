#pragma once

#include "Shader.h"
#include "Texture2D.h"
#include "RenderStateManager.h"
#include "Sampler.h"
#include "ConstantBuffer.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <vector>

namespace Engine
{
	class ConstantBufferManager;

	class Material
	{
	public:
		Material();
		~Material();

		void SetName(const std::string& name) { m_name = name; }
		void SetShader(std::shared_ptr<Shader> shader) { m_pShader = shader; }
		void SetRenderState(const RenderState& renderState) { m_renderState = renderState; }
		void SetColor(const Vector4 v) { m_cbPerMaterial.color = v; }

		const std::string& GetName() const { return m_name; }
		std::shared_ptr<Texture2D> GetTexture(UINT slot) const;
		const std::vector<std::shared_ptr<Texture2D>>& GetTextures() const { return m_textures; }
		Vector4 GetColor() const { return m_cbPerMaterial.color; }
		
		std::shared_ptr<Material> CreateClone() const;
		void SetTexture(UINT slot, std::shared_ptr<Texture2D> texture);
		void SetSampler(UINT slot, std::shared_ptr<Sampler> sampler);
		void Bind(ConstantBufferManager& cbManager);

	private:
		std::shared_ptr<Shader> m_pShader;
		std::vector<std::shared_ptr<Texture2D>> m_textures;
		std::vector<std::shared_ptr<Sampler>> m_samplers;
		std::string m_name;

		// render states
		RenderState m_renderState;

		// default color
		ConstantBufferPerMaterial m_cbPerMaterial;
	};
}