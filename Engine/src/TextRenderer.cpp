#include "pch.h"
#include "TextRenderer.h"
#include "Shader.h"
#include "ConstantBufferManager.h"
#include "D3DManager.h"
#include "ResourceManager.h"
#include "SamplerStateManager.h"
#include "RenderStateManager.h"
#include "RenderCommandList.h"
#include "TextComponent.h"
#include "Font.h"

namespace Engine
{
	void TextRenderer::Initialize()
	{
		m_shader = ResourceManager::GetInstance().GetShader("Text_shader");
		m_quadMesh = ResourceManager::GetInstance().GetMesh("Primitive_quad");
	}

	void TextRenderer::Render(const std::vector<TextComponent*>& texts, ConstantBufferManager* cbManager)
	{
		RenderCommandList cmd(D3DManager::GetInstance().GetDeviceContext());

		m_shader->Bind();

		// set render state
		cmd.SetBlendState(RenderStateManager::GetInstance().GetBlendState(BlendType::Alpha));
		cmd.SetDepthState(RenderStateManager::GetInstance().GetDepthState(DepthType::Disable));
		cmd.SetRasterState(RenderStateManager::GetInstance().GetRasterState(RasterType::None));

		for (const auto* text : texts)
		{
			if (!text->ownerGameObject->IsEnable())
			{
				continue;
			}

			auto* font = text->GetFont();
			auto fontSize = text->GetFontSize();
			auto* atlas = font->GetAtlas(fontSize);
			
			// bind atlas texture
			atlas->Bind(0);
			SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp)->Bind(0);

			// render text
			auto position = text->ownerGameObject->transform.GetWorldPosition();
			float cursorX = position.x;
			auto scale = text->ownerGameObject->transform.GetWorldScale();
			for (char32_t character : text->GetText())
			{
				auto* glyph = font->GetGlyph(character, fontSize);
				// update constant buffer
				ConstantBufferPerText data;
				
				data.position = { cursorX + glyph->bearing.x * scale.x, position.y - glyph->bearing.y * scale.y };
				cursorX += glyph->advance * scale.x;

				data.scale = { glyph->size.x * scale.x, glyph->size.y * scale.y };

				data.uvMin = glyph->uvMin;
				data.uvMax = glyph->uvMax;

				data.color = text->GetColor();

				cbManager->UpdatePerText(data);

				// draw
				m_quadMesh->Render();
			}
		}
	}
}