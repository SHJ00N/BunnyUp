#include "TitleSceneInput.h"
#include "Input/PlayerInputManager.h"

namespace Game
{
    void TitleSceneInput::Update(float dt)
    {
        auto& inputState = PlayerInputManager::GetInstance().GetInputState();
        if (inputState.start)
        {
            Engine::SceneManager::GetInstance().LoadScene("GameScene");
        }
    }
}