#pragma once

#include "Texture2D.h"
#include "MathHelper.h"
#include <memory>

namespace Engine
{
    class SpriteSheet
    {
    public:
        SpriteSheet(std::shared_ptr<Texture2D> texture, uint32_t columns, uint32_t rows);

        const std::shared_ptr<Texture2D>& GetTexture() const;

        Vector2 GetFrameSize() const;
        Vector2 GetFrameOffset(uint32_t frame) const;
        uint32_t GetFrameCount() const;

    private:
        std::shared_ptr<Texture2D> m_texture;

        uint32_t m_columns;
        uint32_t m_rows;

        Vector2 m_frameSize;
    };
}