#include <EngineSystem.h>

void LoadGameResources()
{
	auto& resourceManager = Engine::ResourceManager::GetInstance();

    // Environment map
    resourceManager.LoadEnvironmentMap("Sky_EnvMap", "resources\\textures\\FS002_Day_Sunless.PNG");

    // Chibi_Rabbit
    // ---------------------------------------------------------------------------------------------------
    // Models
	auto bunnyModel = resourceManager.LoadModel("Chibi_Rabbit", "resources\\models\\Chibi_Rabbit\\Chibi_Rabbit.fbx");
    resourceManager.LoadModel("Chibi_Gift_A", "resources\\models\\Environment\\Gift_A.fbx");
    resourceManager.LoadModel("Chibi_Gift_B", "resources\\models\\Environment\\Gift_B.fbx");
    resourceManager.LoadModel("Chibi_Gift_C", "resources\\models\\Environment\\Gift_C.fbx");
    resourceManager.LoadModel("Chibi_Grass_A", "resources\\models\\Environment\\Grass_A.fbx");
    resourceManager.LoadModel("Chibi_Snowman", "resources\\models\\Environment\\Snowman.fbx");
    resourceManager.LoadModel("Chibi_Tree_B", "resources\\models\\Environment\\Tree_B.fbx");
    resourceManager.LoadModel("Chibi_Tree_C", "resources\\models\\Environment\\Tree_C.fbx");

	// Textures
    resourceManager.LoadTexture("T_Snow_Ground", "resources\\textures\\snow_ground.jpg", Engine::TextureType::Albedo);
	resourceManager.LoadTexture("T_Chibi_Rabbit_03", "resources\\models\\Chibi_Rabbit\\Rabbit_Color\\T_Chibi_Rabbit_03.png", Engine::TextureType::Albedo);
	resourceManager.LoadTexture("T_Chibi_Rabbit_06", "resources\\models\\Chibi_Rabbit\\Rabbit_Color\\T_Chibi_Rabbit_06.png", Engine::TextureType::Albedo);
	resourceManager.LoadTexture("T_Chibi_Emo_25", "resources\\models\\Chibi_Rabbit\\Emotion\\T_Chibi_Emo_25.png", Engine::TextureType::Albedo);
	resourceManager.LoadTexture("T_Chibi_Emo_21", "resources\\models\\Chibi_Rabbit\\Emotion\\T_Chibi_Emo_21.png", Engine::TextureType::Albedo);

	// Animations
	resourceManager.LoadAnimation("Chibi_Rabbit_IdleA", "resources\\animations\\Anim_Chibi@IdleA.fbx", bunnyModel.get(), true);
	resourceManager.LoadAnimation("Chibi_Rabbit_IdleC", "resources\\animations\\Anim_Chibi@IdleC.fbx", bunnyModel.get(), true);
	resourceManager.LoadAnimation("Chibi_Rabbit_Walk", "resources\\animations\\Anim_Chibi@Walk.fbx", bunnyModel.get(), true);
	resourceManager.LoadAnimation("Chibi_Rabbit_Run", "resources\\animations\\Anim_Chibi@Run.fbx", bunnyModel.get(), true);
    resourceManager.LoadAnimation("Chibi_Rabbit_Jump", "resources\\animations\\Anim_Chibi@Jump.fbx", bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_Jump_Place", "resources\\animations\\Anim_Chibi@Jump_In_Place.fbx", bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_Fall", "resources\\animations\\Anim_Chibi@Quadruped_Chiling.fbx", bunnyModel.get(), true);
    resourceManager.LoadAnimation("Chibi_Rabbit_Dash", "resources\\animations\\Anim_Chibi@Dash.fbx", bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_Attack", "resources\\animations\\Anim_Chibi@NormalATK.fbx", bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_JumpAttack", "resources\\animations\\Anim_Chibi@Swiming.fbx", bunnyModel.get(), true);
    // Winter Fell
    // ---------------------------------------------------------------------------------------------------
    // Models
    resourceManager.LoadModel("Winter_Barrel_01", "resources\\models\\WinterFell\\Barrel.fbx");
    resourceManager.LoadModel("Winter_Barrel_02", "resources\\models\\WinterFell\\Barrel_02.fbx");
    resourceManager.LoadModel("Winter_Barrel_03", "resources\\models\\WinterFell\\Barrel_03.fbx");
    resourceManager.LoadModel("Winter_Crate", "resources\\models\\WinterFell\\Crate.fbx");
    resourceManager.LoadModel("Winter_Fence", "resources\\models\\WinterFell\\Fence.fbx");
    resourceManager.LoadModel("Winter_Floating_Islend", "resources\\models\\WinterFell\\Floating_Islend.fbx");
    resourceManager.LoadModel("Winter_House", "resources\\models\\WinterFell\\House.fbx");
    resourceManager.LoadModel("Winter_Lights", "resources\\models\\WinterFell\\Lights.fbx");
    resourceManager.LoadModel("Winter_Snow", "resources\\models\\WinterFell\\Snow.fbx");
    resourceManager.LoadModel("Winter_Snowman", "resources\\models\\WinterFell\\Snowman_0.fbx");
    resourceManager.LoadModel("Winter_Stones", "resources\\models\\WinterFell\\Stones.fbx");
    resourceManager.LoadModel("Winter_Tree", "resources\\models\\WinterFell\\Tree.fbx");
    resourceManager.LoadModel("Winter_Tree_Cut", "resources\\models\\WinterFell\\Tree_Cut.fbx");
    resourceManager.LoadModel("Winter_Tree_Lights", "resources\\models\\WinterFell\\Tree_Lights.fbx");
    resourceManager.LoadModel("Winter_Tree2", "resources\\models\\WinterFell\\Tree2.fbx");
    resourceManager.LoadModel("Winter_Tree2_Lights", "resources\\models\\WinterFell\\Tree2_Lights.fbx");
    resourceManager.LoadModel("Winter_Trunk", "resources\\models\\WinterFell\\Trunk.fbx");

    // Textures
    // ---------------------------------------------------------------------------------------------------
    resourceManager.LoadTexture("Snow_Floor_Color", "resources\\textures\\Snow_Floor\\snow_01_color_2k.png", Engine::TextureType::Albedo);
    resourceManager.LoadTexture("Snow_Floor_Normal", "resources\\textures\\Snow_Floor\\snow_01_normal_dx_2k.png", Engine::TextureType::Normal);
    resourceManager.LoadTexture("Snow_Floor_Roughness", "resources\\textures\\Snow_Floor\\snow_01_roughness_2k.png", Engine::TextureType::Default);
    resourceManager.LoadTexture("Snow_Floor_Ambient_Occlusion", "resources\\textures\\Snow_Floor\\snow_01_ambient_occlusion_2k.png", Engine::TextureType::Default);
}