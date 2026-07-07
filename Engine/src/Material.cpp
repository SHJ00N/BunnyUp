#include "pch.h"
#include "Material.h"
#include "D3DManager.h"
#include "ResourceManager.h"
#include "ConstantBufferManager.h"

namespace Engine
{
	Material::Material() : m_pShader(nullptr)
	{
	}

	Material::~Material()
	{
	}

	std::shared_ptr<Material> Material::CreateClone() const
	{
		auto mat = std::make_shared<Material>();

        // Copy member values
		mat->m_pShader = m_pShader;
		mat->m_textures = m_textures;
		mat->m_renderState = m_renderState;
        mat->m_samplers = m_samplers;
        mat->m_name = m_name;

        mat->SetColor(m_cbPerMaterial.color);
        mat->SetMetallic(m_cbPerMaterial.metallic);
        mat->SetRoughness(m_cbPerMaterial.roughness);
        mat->SetHasNormalMap(m_cbPerMaterial.hasNormal);
        mat->SetHasRoughnessMap(m_cbPerMaterial.hasRoughness);
        mat->SetHasMetallicMap(m_cbPerMaterial.hasMetallic);
        mat->SetHasAOMap(m_cbPerMaterial.hasAmbientOcclusion);

		return mat;
	}

	void Material::SetTexture(UINT slot, std::shared_ptr<Texture2D> texture)
	{
        size_t idx = static_cast<size_t>(slot);
        if (m_textures.size() <= idx)
        {
            m_textures.resize(idx + 1);
        }

        m_textures[idx] = texture;
	}

    std::shared_ptr<Texture2D> Material::GetTexture(UINT slot) const
    {
        if (slot >= m_textures.size()) return nullptr;
        return m_textures[slot];
    }

    void Material::SetSampler(UINT slot, std::shared_ptr<Sampler> sampler)
    {
        size_t idx = static_cast<size_t>(slot);
        if (m_samplers.size() <= idx)
        {
            m_samplers.resize(idx + 1);
        }

        m_samplers[idx] = sampler;
	}

	void Material::Bind(ConstantBufferManager& cbManager)
	{
        auto context = D3DManager::GetInstance().GetDeviceContext();

        // Update constant buffer
        cbManager.UpdatePerMaterial(m_cbPerMaterial);

        // Bind shader
        if (m_pShader)
        {
            m_pShader->Bind();
        }

        // Bind Texture
        for (UINT i = 0; i < m_textures.size(); ++i)
        {
            if (m_textures[i])
            {
                m_textures[i]->Bind(i);
            }
        }

		// Bind Sampler
        for (UINT i = 0; i < m_samplers.size(); ++i)
        {
            if (m_samplers[i])
            {
                m_samplers[i]->Bind(i);
            }
        }

        // Bind RenderState
        if (m_renderState.blend)
        {
            float blendFactor[4] = { 0,0,0,0 };
            context->OMSetBlendState(m_renderState.blend.Get(), blendFactor, 0xffffffff);
        }

        if (m_renderState.depth)
        {
            context->OMSetDepthStencilState(m_renderState.depth.Get(), 0);
        }

        if (m_renderState.raster)
        {
            context->RSSetState(m_renderState.raster.Get());
        }
	}
}