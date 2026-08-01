#pragma once

#include <EngineSystem.h>

namespace Game
{
    class TitleSceneInput : public Engine::Component
    {
    public:
        void Update(float dt) override;
    };
}