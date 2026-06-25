#pragma once

#include <EngineSystem.h>

namespace Cube
{
    using namespace Engine;
    class CubeGenerator : public Component
    {
    public:
        void Update(float dt) override;
    private:
        float m_accumulatedTimer = 0.0f;

        std::vector<GameObject*> m_cubes;
    };
}