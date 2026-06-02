#include <EngineSystem.h>

void LoadGameResources()
{
	auto& resourceManager = Engine::ResourceManager::GetInstance();
	// Load  models
	auto bunnyModel = resourceManager.LoadModel("Chibi_Rabbit", "resources\\models\\Chibi_Rabbit\\Chibi_Rabbit.fbx");

	// Load textures
    resourceManager.LoadTexture("T_Snow_Ground", "resources\\textures\\snow_ground.jpg", Engine::TextureType::Albedo);
	resourceManager.LoadTexture("T_Chibi_Rabbit_03", "resources\\models\\Chibi_Rabbit\\Rabbit_Color\\T_Chibi_Rabbit_03.png", Engine::TextureType::Albedo);
	resourceManager.LoadTexture("T_Chibi_Rabbit_06", "resources\\models\\Chibi_Rabbit\\Rabbit_Color\\T_Chibi_Rabbit_06.png", Engine::TextureType::Albedo);
    resourceManager.LoadTexture("T_Chibi_Rabbit_N", "resources\\models\\Chibi_Rabbit\\Rabbit_Color\\T_Chibi_Rabbit_N.png", Engine::TextureType::Normal);
	resourceManager.LoadTexture("T_Chibi_Emo_25", "resources\\models\\Chibi_Rabbit\\Emotion\\T_Chibi_Emo_25.png", Engine::TextureType::Albedo);
	resourceManager.LoadTexture("T_Chibi_Emo_21", "resources\\models\\Chibi_Rabbit\\Emotion\\T_Chibi_Emo_21.png", Engine::TextureType::Albedo);
	// Load animations
	resourceManager.LoadAnimation("Chibi_Rabbit_IdleA", "resources\\animations\\Anim_Chibi@IdleA.fbx", bunnyModel.get(), true);
	resourceManager.LoadAnimation("Chibi_Rabbit_IdleC", "resources\\animations\\Anim_Chibi@IdleC.fbx", bunnyModel.get(), true);
	resourceManager.LoadAnimation("Chibi_Rabbit_Walk", "resources\\animations\\Anim_Chibi@Walk.fbx", bunnyModel.get(), true);
	resourceManager.LoadAnimation("Chibi_Rabbit_Run", "resources\\animations\\Anim_Chibi@Run.fbx", bunnyModel.get(), true);

    resourceManager.LoadEnvironmentMap("Sky_EnvMap", "resources\\textures\\FS002_Day_Sunless.PNG");
}