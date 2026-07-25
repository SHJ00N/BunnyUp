#pragma once

#include "RendererComponent.h"
#include "SpriteSheet.h"

namespace Engine
{
	class Mesh;

	enum class EffectType
	{
		Normal,
		Billboard
	};

	class EffectRenderer : public RendererComponent
	{
	public:
		EffectRenderer(EffectType type);
		virtual ~EffectRenderer() = default;

		void Submit(RenderQueue& queue) override;
		void Draw(const RenderItem& item, ConstantBufferManager& cbManager) override;

		void SetSpriteSheet(std::shared_ptr<Texture2D> texture, uint32_t columns, uint32_t rows);
		void SetCurrentFrame(uint32_t frame) { m_currentFrame = frame; }
		uint32_t GetFrameCount() const { return m_spriteSheet->GetFrameCount(); }

	protected:
		void UpdateConstantBuffer(ConstantBufferManager& cbManager) override;

	private:
		std::shared_ptr<Mesh> m_quadMesh;

		std::shared_ptr<SpriteSheet> m_spriteSheet;
		uint32_t m_currentFrame = 0;

		ConstantBufferPerSpriteSheet m_cbPerSpriteSheet;
	};
}