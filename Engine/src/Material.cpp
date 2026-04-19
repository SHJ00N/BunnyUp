#include "Material.h"
#include "D3DManager.h"
#include "ResourceManager.h"

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

		mat->m_pShader = m_pShader;		// share Shader
		mat->m_textures = m_textures;		// share Texture 
		mat->m_renderState = m_renderState;		// copy RenderState

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

    void Material::SetSampler(UINT slot, std::shared_ptr<Sampler> sampler)
    {
        size_t idx = static_cast<size_t>(slot);
        if (m_samplers.size() <= idx)
        {
            m_samplers.resize(idx + 1);
        }

        m_samplers[idx] = sampler;
	}

	void Material::Bind()
	{
        auto context = D3DManager::GetInstance().GetDeviceContext();

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