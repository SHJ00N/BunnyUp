#include <EngineSystem.h>

void LoadGameResources()
{
	auto& resourceManager = Engine::ResourceManager::GetInstance();

    // Environment map
    resourceManager.LoadEnvironmentMap("Sky_EnvMap", resourceManager.MakePath("resources/textures/FS002_Day_Sunless.PNG").string());

    // Font
    resourceManager.LoadFont("Poly", resourceManager.MakePath("resources/fonts/Poly-Regular.otf").string());
    resourceManager.LoadFont("Square", resourceManager.MakePath("resources/fonts/Square-Black.ttf").string());
    // Effect
    resourceManager.LoadTexture("AHit_Effect", resourceManager.MakePath("resources/textures/Hit-Yellow-A.png").string());
    resourceManager.LoadTexture("BHit_Effect", resourceManager.MakePath("resources/textures/Hit-Yellow-B.png").string());
    resourceManager.LoadTexture("Dust_Effect", resourceManager.MakePath("resources/textures/vapor_cloud.png").string());
    resourceManager.LoadTexture("Star_Effect", resourceManager.MakePath("resources/textures/ice_sparkles.png").string());

    // Chibi_Rabbit
    // ---------------------------------------------------------------------------------------------------
    // Models
	auto bunnyModel = resourceManager.LoadModel("Chibi_Rabbit", resourceManager.MakePath("resources\\models\\Chibi_Rabbit\\Chibi_Rabbit.fbx").string());
    resourceManager.LoadModel("Chibi_Candy_Staff", resourceManager.MakePath("resources\\models\\Chibi_Rabbit\\Candy_cane_staff.fbx").string());
    resourceManager.LoadModel("Chibi_Earmuff", resourceManager.MakePath("resources\\models\\Chibi_Rabbit\\Earmuff.fbx").string());
    resourceManager.LoadModel("Chibi_Scarf", resourceManager.MakePath("resources\\models\\Chibi_Rabbit\\Scarf.fbx").string());
    resourceManager.LoadModel("Candy_Staff", resourceManager.MakePath("resources\\models\\Chibi_Rabbit\\Candy_cane_staff.fbx").string());
    resourceManager.LoadModel("Chibi_Gift_A", resourceManager.MakePath("resources\\models\\Environment\\Gift_A.fbx").string());
    resourceManager.LoadModel("Chibi_Gift_B", resourceManager.MakePath("resources\\models\\Environment\\Gift_B.fbx").string());
    resourceManager.LoadModel("Chibi_Gift_C", resourceManager.MakePath("resources\\models\\Environment\\Gift_C.fbx").string());
    resourceManager.LoadModel("Chibi_Grass_A", resourceManager.MakePath("resources\\models\\Environment\\Grass_A.fbx").string());
    resourceManager.LoadModel("Chibi_Snowman", resourceManager.MakePath("resources\\models\\Environment\\Snowman.fbx").string());
    resourceManager.LoadModel("Chibi_Tree_B", resourceManager.MakePath("resources\\models\\Environment\\Tree_B.fbx").string());
    resourceManager.LoadModel("Chibi_Tree_C", resourceManager.MakePath("resources\\models\\Environment\\Tree_C.fbx").string());

	// Textures
    resourceManager.LoadTexture("T_Snow_Ground", resourceManager.MakePath("resources\\textures\\snow_ground.jpg").string(), Engine::TextureType::Albedo);
	resourceManager.LoadTexture("T_Chibi_Rabbit_03", resourceManager.MakePath("resources\\models\\Chibi_Rabbit\\Rabbit_Color\\T_Chibi_Rabbit_03.png").string(), Engine::TextureType::Albedo);
	resourceManager.LoadTexture("T_Chibi_Rabbit_06", resourceManager.MakePath("resources\\models\\Chibi_Rabbit\\Rabbit_Color\\T_Chibi_Rabbit_06.png").string(), Engine::TextureType::Albedo);
	resourceManager.LoadTexture("T_Chibi_Emo_25", resourceManager.MakePath("resources\\models\\Chibi_Rabbit\\Emotion\\T_Chibi_Emo_25.png").string(), Engine::TextureType::Albedo);
	resourceManager.LoadTexture("T_Chibi_Emo_21", resourceManager.MakePath("resources\\models\\Chibi_Rabbit\\Emotion\\T_Chibi_Emo_21.png").string(), Engine::TextureType::Albedo);

	// Animations
	resourceManager.LoadAnimation("Chibi_Rabbit_IdleA", resourceManager.MakePath("resources\\animations\\Chibi_Rabbit\\Anim_Chibi@IdleA.fbx").string(), bunnyModel.get(), true);
    resourceManager.LoadAnimation("Chibi_Rabbit_IdleB", resourceManager.MakePath("resources\\animations\\Chibi_Rabbit\\Anim_Chibi@IdleB.fbx").string(), bunnyModel.get(), true);
	resourceManager.LoadAnimation("Chibi_Rabbit_IdleC", resourceManager.MakePath("resources\\animations\\Chibi_Rabbit\\Anim_Chibi@IdleC.fbx").string(), bunnyModel.get(), true);
    resourceManager.LoadAnimation("Chibi_Rabbit_Idle03", resourceManager.MakePath("resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Idle03.fbx").string(), bunnyModel.get(), true);
	resourceManager.LoadAnimation("Chibi_Rabbit_Walk", resourceManager.MakePath("resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Walk.fbx").string(), bunnyModel.get(), true);
	resourceManager.LoadAnimation("Chibi_Rabbit_Run", resourceManager.MakePath("resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Run.fbx").string(), bunnyModel.get(), true);
    resourceManager.LoadAnimation("Chibi_Rabbit_Jump", resourceManager.MakePath("resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Jump.fbx").string(), bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_Jump_Place", resourceManager.MakePath("resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Jump_In_Place.fbx").string(), bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_Fall", resourceManager.MakePath("resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Quadruped_Chiling.fbx").string(), bunnyModel.get(), true);
    resourceManager.LoadAnimation("Chibi_Rabbit_Dash", resourceManager.MakePath("resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Dash.fbx").string(), bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_Attack", resourceManager.MakePath("resources\\animations\\Chibi_Rabbit\\Anim_Chibi@NormalATK.fbx").string(), bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_Damage", resourceManager.MakePath("resources\\animations\\Chibi_Rabbit\\Anim_Chibi@Damage.fbx").string(), bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_DeathA", resourceManager.MakePath("resources\\animations\\Chibi_Rabbit\\Anim_Chibi@DieA.fbx").string(), bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_DeathB", resourceManager.MakePath("resources\\animations\\Chibi_Rabbit\\Anim_Chibi@DieB.fbx").string(), bunnyModel.get(), false);
    resourceManager.LoadAnimation("Chibi_Rabbit_DeathC", resourceManager.MakePath("resources\\animations\\Chibi_Rabbit\\Anim_Chibi@DieC.fbx").string(), bunnyModel.get(), false);


    // Winter Fell
    // ---------------------------------------------------------------------------------------------------
    // Models
    resourceManager.LoadModel("Winter_Barrel_01", resourceManager.MakePath("resources\\models\\WinterFell\\Barrel.fbx").string());
    resourceManager.LoadModel("Winter_Barrel_02", resourceManager.MakePath("resources\\models\\WinterFell\\Barrel_02.fbx").string());
    resourceManager.LoadModel("Winter_Barrel_03", resourceManager.MakePath("resources\\models\\WinterFell\\Barrel_03.fbx").string());
    resourceManager.LoadModel("Winter_Crate", resourceManager.MakePath("resources\\models\\WinterFell\\Crate.fbx").string());
    resourceManager.LoadModel("Winter_Fence", resourceManager.MakePath("resources\\models\\WinterFell\\Fence.fbx").string());
    resourceManager.LoadModel("Winter_Floating_Islend", resourceManager.MakePath("resources\\models\\WinterFell\\Floating_Islend.fbx").string());
    resourceManager.LoadModel("Winter_House", resourceManager.MakePath("resources\\models\\WinterFell\\House.fbx").string());
    resourceManager.LoadModel("Winter_Lights", resourceManager.MakePath("resources\\models\\WinterFell\\Lights.fbx").string());
    resourceManager.LoadModel("Winter_Snow", resourceManager.MakePath("resources\\models\\WinterFell\\Snow.fbx").string());
    resourceManager.LoadModel("Winter_Snowman", resourceManager.MakePath("resources\\models\\WinterFell\\Snowman_0.fbx").string());
    resourceManager.LoadModel("Winter_Stones", resourceManager.MakePath("resources\\models\\WinterFell\\Stones.fbx").string());
    resourceManager.LoadModel("Winter_Tree", resourceManager.MakePath("resources\\models\\WinterFell\\Tree.fbx").string());
    resourceManager.LoadModel("Winter_Tree_Cut", resourceManager.MakePath("resources\\models\\WinterFell\\Tree_Cut.fbx").string());
    resourceManager.LoadModel("Winter_Tree_Lights", resourceManager.MakePath("resources\\models\\WinterFell\\Tree_Lights.fbx").string());
    resourceManager.LoadModel("Winter_Tree2", resourceManager.MakePath("resources\\models\\WinterFell\\Tree2.fbx").string());
    resourceManager.LoadModel("Winter_Tree2_Lights", resourceManager.MakePath("resources\\models\\WinterFell\\Tree2_Lights.fbx").string());
    resourceManager.LoadModel("Winter_Trunk", resourceManager.MakePath("resources\\models\\WinterFell\\Trunk.fbx").string());

    // Textures
    // ---------------------------------------------------------------------------------------------------
    resourceManager.LoadTexture("Snow_Floor_Color", resourceManager.MakePath("resources\\textures\\Snow_Floor\\snow_01_color_2k.png").string(), Engine::TextureType::Albedo);
    resourceManager.LoadTexture("Snow_Floor_Normal", resourceManager.MakePath("resources\\textures\\Snow_Floor\\snow_01_normal_dx_2k.png").string(), Engine::TextureType::Normal);
    resourceManager.LoadTexture("Snow_Floor_Roughness", resourceManager.MakePath("resources\\textures\\Snow_Floor\\snow_01_roughness_2k.png").string(), Engine::TextureType::Default);
    resourceManager.LoadTexture("Snow_Floor_Ambient_Occlusion", resourceManager.MakePath("resources\\textures\\Snow_Floor\\snow_01_ambient_occlusion_2k.png").string(), Engine::TextureType::Default);

    // Monster
    // ---------------------------------------------------------------------------------------------------
    // Models
    auto slime = resourceManager.LoadModel("Monster_Slime", resourceManager.MakePath("resources\\models\\Monster\\SlimeMesh.fbx").string());
    auto turtleShell = resourceManager.LoadModel("Monster_TurtleShell", resourceManager.MakePath("resources\\models\\Monster\\TurtleShellMesh.fbx").string());

    // Animations
    resourceManager.LoadAnimation("Monster_Slime_Idle", resourceManager.MakePath("resources\\animations\\Monster\\Slime\\IdleNormal_Slime_Anim.fbx").string(), slime.get(), true);
    resourceManager.LoadAnimation("Monster_Slime_Walk", resourceManager.MakePath("resources\\animations\\Monster\\Slime\\Walk_Slime_Anim.fbx").string(), turtleShell.get(), true);
    resourceManager.LoadAnimation("Monster_Slime_Attack", resourceManager.MakePath("resources\\animations\\Monster\\Slime\\Attack01_Slime_Anim.fbx").string(), turtleShell.get(), false);
    resourceManager.LoadAnimation("Monster_Slime_Damage", resourceManager.MakePath("resources\\animations\\Monster\\Slime\\GetHit_Slime_Anim.fbx").string(), turtleShell.get(), false);
    resourceManager.LoadAnimation("Monster_Slime_Death", resourceManager.MakePath("resources\\animations\\Monster\\Slime\\Die_Slime_Anim.fbx").string(), turtleShell.get(), false);

    resourceManager.LoadAnimation("Monster_TurtleShell_Idle", resourceManager.MakePath("resources\\animations\\Monster\\TurtleShell\\IdleBattle_TurtleShell_Anim.fbx").string(), turtleShell.get(), true);
    resourceManager.LoadAnimation("Monster_TurtleShell_Walk", resourceManager.MakePath("resources\\animations\\Monster\\TurtleShell\\Walk_TurtleShell_Anim.fbx").string(), turtleShell.get(), true);
    resourceManager.LoadAnimation("Monster_TurtleShell_Attack1", resourceManager.MakePath("resources\\animations\\Monster\\TurtleShell\\Attack01_TurtleShell_Anim.fbx").string(), turtleShell.get(), false);
    resourceManager.LoadAnimation("Monster_TurtleShell_Attack2", resourceManager.MakePath("resources\\animations\\Monster\\TurtleShell\\Attack02_TurtleShell_Anim.fbx").string(), turtleShell.get(), false);
    resourceManager.LoadAnimation("Monster_TurtleShell_Damage", resourceManager.MakePath("resources\\animations\\Monster\\TurtleShell\\GetHit_TurtleShell_Anim.fbx").string(), turtleShell.get(), false);
    resourceManager.LoadAnimation("Monster_TurtleShell_Death", resourceManager.MakePath("resources\\animations\\Monster\\TurtleShell\\Die_TurtleShell_Anim.fbx").string(), turtleShell.get(), false);

    // Common
    resourceManager.LoadModel("Thron", resourceManager.MakePath("resources\\models\\Weapon\\Thron.fbx").string());
    resourceManager.LoadModel("Heart", resourceManager.MakePath("resources\\models\\Item\\Heart.fbx").string());
}