#include "pch.h"
#include "Font.h"
#include "Texture2D.h"
#include "Log.h"

#include "ft2build.h"
#include FT_FREETYPE_H

namespace Engine
{
	bool Font::Load(const std::string& path)
	{
		m_path = path;

		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			LOG_INFO("Coult not init FreeType Library");
			return false;
		}

		FT_Face face;
		if (FT_New_Face(ft, path.c_str(), 0, &face))
		{
			LOG_INFO("Failed to load font : %s", path.c_str());
			return false;
		}

		// destroy FreeType once we're finished
		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		return true;
	}

	const Glyph* Font::GetGlyph(char32_t character,	uint32_t fontSize)
	{
		auto it = m_atlases.find(fontSize);

		if (it == m_atlases.end())
		{
			if (!buildAtlas(fontSize))
			{
				return nullptr;
			}

			it = m_atlases.find(fontSize);

			if (it == m_atlases.end())
			{
				return nullptr;
			}
		}

		auto glyphIt = it->second.glyphs.find(character);

		if (glyphIt == it->second.glyphs.end())
		{
			return nullptr;
		}

		return &glyphIt->second;
	}

	Texture2D* Font::GetAtlas(uint32_t fontSize)
	{
		auto it = m_atlases.find(fontSize);

		if (it == m_atlases.end())
		{
			if (!buildAtlas(fontSize))
			{
				return nullptr;
			}

			it = m_atlases.find(fontSize);

			if (it == m_atlases.end())
			{
				return nullptr;
			}
		}

		return it->second.texture.get();
	}

	bool Font::buildAtlas(uint32_t fontSize)
	{
		if (fontSize == 0)
		{
			return false;
		}

		if (m_atlases.contains(fontSize))
		{
			return true;
		}

		// Init freetype
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			LOG_INFO("Coult not init FreeType Library");
			return false;
		}

		FT_Face face;
		if (FT_New_Face(ft, m_path.c_str(), 0, &face))
		{
			LOG_INFO("Failed to load font : %s", m_path.c_str());
			return false;
		}

		if (FT_Set_Pixel_Sizes(face, 0, fontSize))
		{
			LOG_INFO("Failed to set font size : %u", fontSize);
			FT_Done_Face(face);
			FT_Done_FreeType(ft);
			return false;
		}

		// calculate atlas size
		constexpr uint32_t maxRowWidth = 1024;
		constexpr uint32_t padding = 2;

		uint32_t atlasWidth = 0;
		uint32_t atlasHeight = 0;

		uint32_t cursorX = padding;
		uint32_t cursorY = padding;

		uint32_t rowHeight = 0;

		for (char32_t c = 0; c < 128; ++c)
		{
			// load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				LOG_INFO("Failed to load glyph : %u", static_cast<uint32_t>(c));
				continue;
			}

			// calculate glyph texture data on atlas
			const auto& bitmap = face->glyph->bitmap;

			uint32_t glyphWidth = static_cast<uint32_t>(bitmap.width);
			uint32_t glyphHeight = static_cast<uint32_t>(bitmap.rows);

			uint32_t requiredWidth =	glyphWidth + padding;
			uint32_t requiredHeight = glyphHeight + padding;

			// if current line is full, go to next line
			if (cursorX + requiredWidth > maxRowWidth)
			{
				cursorX = padding;
				cursorY += rowHeight + padding;
				rowHeight = 0;
			}

			cursorX += requiredWidth;

			rowHeight = std::max(rowHeight, requiredHeight);
			atlasWidth = std::max(atlasWidth, cursorX + padding);
		}

		atlasHeight = cursorY + rowHeight + padding;

		if (atlasWidth == 0 || atlasHeight == 0)
		{
			LOG_INFO("Failed to calculate font atlas size");

			FT_Done_Face(face);
			FT_Done_FreeType(ft);
			return false;
		}

		// create cpu atlas buffer
		std::vector<uint8_t> atlasPixels(static_cast<size_t>(atlasWidth) * static_cast<size_t>(atlasHeight), 0);

		// copy glyph bitmap to atlas
		FontAtlas atlas;

		atlas.width = atlasWidth;
		atlas.height = atlasHeight;

		cursorX = padding;
		cursorY = padding;
		rowHeight = 0;

		for (char32_t c = 0; c <= 126; ++c)
		{
			if (FT_Load_Char(face, static_cast<FT_ULong>(c), FT_LOAD_RENDER))
			{
				continue;
			}

			const FT_GlyphSlot glyph = face->glyph;
			const FT_Bitmap& bitmap = glyph->bitmap;

			const uint32_t glyphWidth = static_cast<uint32_t>(bitmap.width);
			const uint32_t glyphHeight = static_cast<uint32_t>(bitmap.rows);

			const uint32_t requiredWidth = glyphWidth + padding;
			const uint32_t requiredHeight = glyphHeight + padding;

			if (cursorX + requiredWidth > maxRowWidth)
			{
				cursorX = padding;
				cursorY += rowHeight + padding;
				rowHeight = 0;
			}

			// freetype bitmap to atlas
			for (uint32_t y = 0; y < glyphHeight; ++y)
			{
				const uint8_t* src = bitmap.buffer + y * bitmap.pitch;
				uint8_t* dst = atlasPixels.data() + (cursorY + y) * atlasWidth + cursorX;

				std::memcpy(dst, src, glyphWidth);
			}

			// store glyph info
			Glyph glyphData;

			glyphData.uvMin = Vector2(
				static_cast<float>(cursorX) / static_cast<float>(atlasWidth),
				static_cast<float>(cursorY) / static_cast<float>(atlasHeight));
			glyphData.uvMax = Vector2(
				static_cast<float>(cursorX + glyphWidth) / static_cast<float>(atlasWidth),
				static_cast<float>(cursorY + glyphHeight) / static_cast<float>(atlasHeight));

			glyphData.size = Vector2(static_cast<float>(glyphWidth), static_cast<float>(glyphHeight));
			glyphData.bearing = Vector2(static_cast<float>(glyph->bitmap_left), static_cast<float>(glyph->bitmap_top));
			glyphData.advance = static_cast<float>(glyph->advance.x) / 64.0f;

			atlas.glyphs.emplace(c, glyphData);

			cursorX += requiredWidth;
			rowHeight = std::max(rowHeight, requiredHeight);
		}

		// create atlas texture
		atlas.texture = std::make_unique<Texture2D>();

		if (FAILED(atlas.texture->CreateFromMemory(m_path + " " + std::to_string(fontSize), atlasWidth, atlasHeight, DXGI_FORMAT_R8_UNORM, atlasPixels.data(), atlasWidth)))
		{
			LOG_INFO("Failed to create font atlas texture : %u", fontSize);

			FT_Done_Face(face);
			FT_Done_FreeType(ft);
			return false;
		}

		m_atlases.emplace(fontSize, std::move(atlas));

		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		return true;
	}
}