#pragma once

#include <EngineSystem.h>

namespace Game
{
    using namespace Engine;
    class CubeController :public Component
    {
    protected:
        void OnStart() override;
    };
}