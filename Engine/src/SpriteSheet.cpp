#include "pch.h"
#include "SpriteSheet.h"

namespace Engine
{
    SpriteSheet::SpriteSheet(std::shared_ptr<Texture2D> texture, uint32_t columns, uint32_t rows)
        :m_texture(texture), m_columns(columns), m_rows(rows)
    {
        m_frameSize = { 1.0f / columns, 1.0f / rows };
    }

    const std::shared_ptr<Texture2D>& SpriteSheet::GetTexture() const
    {
        return m_texture;
    }

    Vector2 SpriteSheet::GetFrameSize() const
    {
        return m_frameSize;
    }

    Vector2 SpriteSheet::GetFrameOffset(uint32_t frame) const
    {
        uint32_t x = frame % m_columns;
        uint32_t y = frame / m_columns;

        return { x * m_frameSize.x, y * m_frameSize.y };
    }

    uint32_t SpriteSheet::GetFrameCount() const
    {
        return m_columns * m_rows;
    }
}