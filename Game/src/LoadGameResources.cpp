#include <EngineSystem.h>

void LoadGameResources()
{
	auto& resourceManager = Engine::ResourceManager::GetInstance();

    // Environment map
    resourceManager.LoadEnvironmentMap("Sky_EnvMap", "resources\\textures\\FS002_Day_Sunless.PNG");

    // Font
    resourceManager.LoadFont("Poly", "resources\\fonts\\Poly-Regular.otf");

    // Effect
    resourceManager.LoadTexture("AHit_Effect", "resources\\textures\\Hit-Yellow-A.png");
    resourceManager.LoadTexture("BHit_Effect", "resources\\textures\\Hit-Yellow-B.png");
    resourceManager.LoadTexture("Dust_Effect", "resources\\textures\\vapor_cloud.png");

    // Chibi_Rabbit
    // ---------------------------------------------------------------------------------------------------
    // Models
	auto bunnyModel = resourceManager.LoadModel("Chibi_Rabbit", "resources\\models\\Chibi_Rabbit\\Chibi_Rabbit.fbx");
    resourceManager.LoadModel("Chibi_Candy_Staff", "resources\\models\\Chibi_Rabbit\\Candy_cane_staff.fbx");
    resourceManager.LoadModel("Chibi_Earmuff", "resources\\models\\Chibi_Rabbit\\Earmuff.fbx");
    resourceManager.LoadModel("Chibi_Scarf", "resources\\models\\Chibi_Rabbit\\Scarf.fbx");
    resourceManager.LoadModel("Candy_Staff", "resources\\models\\Chibi_Rabbit\\Candy_cane_staff.fbx");
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
	resourceManager.LoadAnimation("Chibi_Rabbit_IdleA", "resources\\animations\\Chibi_Rabbit\\Anim_Chibi@IdleA.fbx", bunnyModel.get(), true);
    resourceManager.LoadAnimation("Chibi_Rabbit_IdleB", "resources\\animations\\Chibi_Rabbit\\Anim_Chibi@IdleB.fbx", bunnyModel.get(), true);
	resourceManager.LoadAnimation("Chibi_Rabbit_IdleC", "resources\\animations\\Chibi_Rabbit\\Anim_Chibi@IdleC.fbx", bunnyModel.get(), true);
    resourceManager.LoadAnimation("Chibi_Rabbit_Idle03", "resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Idle03.fbx", bunnyModel.get(), true);
	resourceManager.LoadAnimation("Chibi_Rabbit_Walk", "resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Walk.fbx", bunnyModel.get(), true);
	resourceManager.LoadAnimation("Chibi_Rabbit_Run", "resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Run.fbx", bunnyModel.get(), true);
    resourceManager.LoadAnimation("Chibi_Rabbit_Jump", "resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Jump.fbx", bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_Jump_Place", "resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Jump_In_Place.fbx", bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_Fall", "resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Quadruped_Chiling.fbx", bunnyModel.get(), true);
    resourceManager.LoadAnimation("Chibi_Rabbit_Dash", "resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Dash.fbx", bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_Attack", "resources\\animations\\Chibi_Rabbit\\Anim_Chibi@NormalATK.fbx", bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_Damage", "resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Damage.fbx", bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_DeathA", "resources\\animations\\Chibi_Rabbit\\Anim_Chibi@DieA.fbx", bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_DeathB", "resources\\animations\\Chibi_Rabbit\\Anim_Chibi@DieB.fbx", bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_DeathC", "resources\\animations\\Chibi_Rabbit\\Anim_Chibi@DieC.fbx", bunnyModel.get(), false);


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

    // Monster
    // ---------------------------------------------------------------------------------------------------
    // Models
    auto slime = resourceManager.LoadModel("Monster_Slime", "resources\\models\\Monster\\SlimeMesh.fbx");
    auto turtleShell = resourceManager.LoadModel("Monster_TurtleShell", "resources\\models\\Monster\\TurtleShellMesh.fbx");

    // Animations
    resourceManager.LoadAnimation("Monster_Slime_Idle", "resources\\animations\\Monster\\Slime\\IdleNormal_Slime_Anim.fbx", slime.get(), true);
    resourceManager.LoadAnimation("Monster_Slime_Walk", "resources\\animations\\Monster\\Slime\\Walk_Slime_Anim.fbx", turtleShell.get(), true);
    resourceManager.LoadAnimation("Monster_Slime_Attack", "resources\\animations\\Monster\\Slime\\Attack01_Slime_Anim.fbx", turtleShell.get(), false);
    resourceManager.LoadAnimation("Monster_Slime_Damage", "resources\\animations\\Monster\\Slime\\GetHit_Slime_Anim.fbx", turtleShell.get(), false);
    resourceManager.LoadAnimation("Monster_Slime_Death", "resources\\animations\\Monster\\Slime\\Die_Slime_Anim.fbx", turtleShell.get(), false);

    resourceManager.LoadAnimation("Monster_TurtleShell_Idle", "resources\\animations\\Monster\\TurtleShell\\IdleBattle_TurtleShell_Anim.fbx", turtleShell.get(), true);
    resourceManager.LoadAnimation("Monster_TurtleShell_Walk", "resources\\animations\\Monster\\TurtleShell\\Walk_TurtleShell_Anim.fbx", turtleShell.get(), true);
    resourceManager.LoadAnimation("Monster_TurtleShell_Attack1", "resources\\animations\\Monster\\TurtleShell\\Attack01_TurtleShell_Anim.fbx", turtleShell.get(), false);
    resourceManager.LoadAnimation("Monster_TurtleShell_Attack2", "resources\\animations\\Monster\\TurtleShell\\Attack02_TurtleShell_Anim.fbx", turtleShell.get(), false);
    resourceManager.LoadAnimation("Monster_TurtleShell_Damage", "resources\\animations\\Monster\\TurtleShell\\GetHit_TurtleShell_Anim.fbx", turtleShell.get(), false);
    resourceManager.LoadAnimation("Monster_TurtleShell_Death", "resources\\animations\\Monster\\TurtleShell\\Die_TurtleShell_Anim.fbx", turtleShell.get(), false);

    // Weapon
    // Model
    resourceManager.LoadModel("Thron", "resources\\models\\Weapon\\Thron.fbx");
}