#include "ResourceManager.h"

void LoadGameResources()
{
	auto& resourceManager = Engine::ResourceManager::GetInstance();
	// Load  models
	auto bunnyModel = resourceManager.LoadModel("Chibi_Rabbit", "C:\\Project\\BunnyUp\\Engine\\resources\\models\\Chibi_Rabbit.fbx");

	// Load textures
	resourceManager.LoadTexture("T_Chibi_Rabbit_03", "C:\\Project\\BunnyUp\\Engine\\resources\\models\\Textures\\T_Chibi_Rabbit_03.png");
	resourceManager.LoadTexture("T_Chibi_Rabbit_06", "C:\\Project\\BunnyUp\\Engine\\resources\\models\\Textures\\T_Chibi_Rabbit_06.png");
	resourceManager.LoadTexture("T_Chibi_Emo_25", "C:\\Project\\BunnyUp\\Engine\\resources\\models\\Textures\\T_Chibi_Emo_25.png");
	resourceManager.LoadTexture("T_Chibi_Emo_21", "C:\\Project\\BunnyUp\\Engine\\resources\\models\\Textures\\T_Chibi_Emo_21.png");

	// Load animations
	resourceManager.LoadAnimation("Chibi_Rabbit_IdleA", "C:\\Project\\BunnyUp\\Engine\\resources\\animations\\Anim_Chibi@IdleA.fbx", bunnyModel.get(), true);
	resourceManager.LoadAnimation("Chibi_Rabbit_IdleC", "C:\\Project\\BunnyUp\\Engine\\resources\\animations\\Anim_Chibi@IdleC.fbx", bunnyModel.get(), true);
	resourceManager.LoadAnimation("Chibi_Rabbit_Walk", "C:\\Project\\BunnyUp\\Engine\\resources\\animations\\Anim_Chibi@Walk.fbx", bunnyModel.get(), true);
	resourceManager.LoadAnimation("Chibi_Rabbit_Run", "C:\\Project\\BunnyUp\\Engine\\resources\\animations\\Anim_Chibi@Run.fbx", bunnyModel.get(), true);
}