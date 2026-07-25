#include "pch.h"
#include "EffectRenderer.h"
#include "Mesh.h"
#include "RenderQueue.h"
#include "ConstantBufferManager.h"
#include "ImGuiClass.h"
#include "ResourceManager.h"

namespace Engine
{
	EffectRenderer::EffectRenderer(EffectType type)
	{
		switch (type)
		{
		case EffectType::Billboard:
		{
			m_quadMesh = ResourceManager::GetInstance().GetMesh("Primitive_quad");
			SetMaterial(0, ResourceManager::GetInstance().GetMaterial("Screen_Billboard_material")->CreateClone());
			break;
		}
		case EffectType::Normal:
		{
			m_quadMesh = ResourceManager::GetInstance().GetMesh("Primitive_floor_quad");
			SetMaterial(0, ResourceManager::GetInstance().GetMaterial("Normal_Effect_material")->CreateClone());
			break;
		}
		}
	}

	void EffectRenderer::SetSpriteSheet(std::shared_ptr<Texture2D> texture, uint32_t columns, uint32_t rows)
	{
		m_spriteSheet = std::make_shared<SpriteSheet>(texture, columns, rows);
		m_currentFrame = 0;

		GetMaterial(0)->SetTexture(0, texture);
	}

	void EffectRenderer::Submit(RenderQueue& queue)
	{
		queue.AddTransparent({ this, 0, 0 });
	}

	void EffectRenderer::Draw(const RenderItem& item, ConstantBufferManager& cbManager)
	{
		UpdateConstantBuffer(cbManager);

		m_quadMesh->RenderSubMesh(0, m_materials, cbManager);
	}

	void EffectRenderer::UpdateConstantBuffer(ConstantBufferManager& cbManager)
	{
		// update object
		m_cbPerObject.world = ownerGameObject->transform.GetWorldMatrix();
		m_cbPerObject.normalMatrix = Transpose(Inverse(m_cbPerObject.world));
		cbManager.UpdatePerObject(m_cbPerObject);

		// update billboard
		if (m_spriteSheet)
		{
			m_cbPerSpriteSheet.frameSize = m_spriteSheet->GetFrameSize();
			m_cbPerSpriteSheet.frameOffset = m_spriteSheet->GetFrameOffset(m_currentFrame);
			cbManager.UpdatePerSpriteSheet(m_cbPerSpriteSheet);
		}
	}
}