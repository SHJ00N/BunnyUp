#pragma once

#include "Component.h"

#include <memory>
#include <string>

namespace Engine
{
	class Font;
	class TextComponent : public Component
	{
	public:
		void SetText(const std::string& text) { m_text = text; }
		void SetFont(std::shared_ptr<Font> font) { m_font = font; }
		void SetFontSize(uint32_t fontSize) { m_fontSize = fontSize; }
		void SetColor(const Vector4& color) { m_color = color; }

		const std::string& GetText() const { return m_text; }
		Font* GetFont() const { return m_font.get(); }
		uint32_t GetFontSize() const { return m_fontSize; }
		const Vector4& GetColor() const { return m_color; }

		void OnImGui() override;

	protected:
		void OnAwake() override;
		void OnDestroy() override;

	private:
		std::string m_text;
		std::shared_ptr<Font> m_font;
		uint32_t m_fontSize = 32;
		Vector4 m_color = Vector4(1.0f);
	};
}