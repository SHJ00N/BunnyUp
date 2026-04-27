#include "SceneFactory.h"
#include "DemoScene1.h"
#include "DemoScene2.h"

void RegistractionGameScenes()
{
	auto& factory = Engine::SceneFactory().GetInstance();

	// registraction demo scene
	factory.Register("DemoScene1", []() { return std::make_unique<Scenes::DemoScene1>(); });
	factory.Register("DemoScene2", []() { return std::make_unique<Scenes::DemoScene2>(); });
}