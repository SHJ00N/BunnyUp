#pragma once

#include "Scene.h"

namespace Game
{
    using namespace Engine;
    class GameScene : public Scene
    {
    public:
        GameScene();
        ~GameScene();

    protected:
        void SceneEnter() override;
    };
}