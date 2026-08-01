#include <EngineSystem.h>

#include "Scene/TitleScene.h"
#include "Input/PlayerInputManager.h"
#include "UI/StartText.h"
#include "TitleSceneInput.h"

namespace Game
{
    TitleScene::TitleScene()
	{
	}

    TitleScene::~TitleScene()
	{
	}

	void TitleScene::SceneEnter()
	{
        SetEnvironmentMap(ResourceManager::GetInstance().GetEnvironmentMap("Sky_EnvMap"));

        auto playerInputManager = CreateGameObject<GameObject>("PlayerInputManager");
        playerInputManager->AddComponent<PlayerInputManager>();

        auto titleSceneInput = CreateGameObject<GameObject>("TitleSceneInput");
        titleSceneInput->AddComponent<TitleSceneInput>();

        auto titleTextObject = CreateGameObject<GameObject>("TitleText");
        titleTextObject->transform.SetLocalPosition(Vector3(110.0f, 500.0f, 0.0f));
        titleTextObject->transform.SetLocalScale(Vector3(2.5f, 2.5f, 1.0f));

        auto titleText = titleTextObject->AddComponent<TextComponent>();
        titleText->SetText("Bunny Engine");
        titleText->SetFont(ResourceManager::GetInstance().GetFont("Square"));
        titleText->SetFontSize(128);
        titleText->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

        auto startTextObject = CreateGameObject<GameObject>("StartText");
        startTextObject->transform.SetLocalPosition(Vector3(450.0f, 800.0f, 0.0f));
        startTextObject->transform.SetLocalScale(Vector3(2.0f, 2.0f, 1.0f));

        startTextObject->AddComponent<StartText>();

        auto startText = startTextObject->AddComponent<TextComponent>();
        startText->SetText("Press Enter to start");
        startText->SetFont(ResourceManager::GetInstance().GetFont("Square"));
        startText->SetFontSize(64);
        startText->SetColor(Vector4(0.8f, 0.8f, 0.8f, 1.0f));
	}
}