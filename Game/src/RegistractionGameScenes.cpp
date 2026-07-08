#include <EngineSystem.h>

#include "DemoScene1.h"
#include "DemoScene2.h"
#include "GameScene.h"

void RegistractionGameScenes()
{
	auto& factory = Engine::SceneFactory().GetInstance();

	// registraction demo scene
	factory.Register("DemoScene1", []() { return std::make_unique<Game::DemoScene1>(); });
	factory.Register("DemoScene2", []() { return std::make_unique<Game::DemoScene2>(); });
    factory.Register("GameScene",   []() { return std::make_unique<Game::GameScene>(); });
}