#pragma once

#include "MathHelper.h"

#include <string>
#include <unordered_map>
#include <memory>

namespace Engine
{
	class Texture2D;

	struct Glyph
	{
		Vector2 uvMin;
		Vector2 uvMax;
		
		Vector2 size;	// bitmap size of glyph
		Vector2 bearing;	// glyph position on base line
		float advance = 0.0f;	// distance to next glyph
	};

	struct FontAtlas
	{
		std::unordered_map<char32_t, Glyph> glyphs;
		std::unique_ptr<Texture2D> texture;
		uint32_t width = 0;
		uint32_t height = 0;
	};

	class Font
	{
	public:
		bool Load(const std::string& path);	// test load font
		const Glyph* GetGlyph(char32_t character, uint32_t fontSize);
		Texture2D* GetAtlas(uint32_t fontSize);

	private:
		std::string m_path;
		std::unordered_map<uint32_t, FontAtlas> m_atlases;

		bool buildAtlas(uint32_t fontSize);
	};
}