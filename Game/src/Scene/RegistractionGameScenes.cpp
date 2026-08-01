#include <EngineSystem.h>

#include "Scene/GameScene.h"
#include "Scene/TitleScene.h"

void RegistractionGameScenes()
{
	auto& factory = Engine::SceneFactory().GetInstance();

	// registraction demo scene
	factory.Register("TitleScene", []() { return std::make_unique<Game::TitleScene>(); });
	factory.Register("GameScene", []() { return std::make_unique<Game::GameScene>(); });
}