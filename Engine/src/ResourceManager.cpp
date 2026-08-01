#include "pch.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "Model.h"
#include "Animation.h"
#include "PrimitiveMeshFactory.h"
#include "EnvironmentMap.h"
#include "SamplerStateManager.h"
#include "Font.h"

namespace Engine
{
	ResourceManager::ResourceManager() : m_rootPath("Assets")
	{
	}

	ResourceManager::~ResourceManager()
	{
	}

	std::filesystem::path ResourceManager::MakePath(const std::string& path) const
	{
		return std::filesystem::path(m_rootPath) / path;
	}

	void ResourceManager::LoadDefaultResources()
	{
		// Load default shader
		// ---------------------------------------------------------------------
		// mesh shader
		LoadShader<VertexPNUT>("Default_shader", MakePath("shaders/BasicShader.hlsl").string());
		LoadShader<VertexPNUT>("Textured_shader", MakePath("shaders/TextureShader.hlsl").string());
		LoadShader<VertexPNUT>("Forward_Textured_shader", MakePath("shaders/ForwardTexturedShader.hlsl").string());
		LoadShader<VertexPNUT>("Screen_Billboard_shader", MakePath("shaders/ScreenBillboardShader.hlsl").string());
		LoadShader<VertexPNUT>("Normal_Effect_shader", MakePath("shaders/NormalEffectShader.hlsl").string());
		LoadShader<VertexPNUT>("Text_shader", MakePath("shaders/TextShader.hlsl").string());
		LoadShader<VertexSkin>("Skinning_shader", MakePath("shaders/SkinningShader.hlsl").string());
		LoadShader<VertexSkin>("Forward_Skinning_shader", MakePath("shaders/ForwardSkinningShader.hlsl").string());
		// render target shader
		LoadShader<VertexPU>("PBR_shader", MakePath("shaders/PBR_ToonShader.hlsl").string());
		LoadShader<VertexPU>("PostProcess_shader", MakePath("shaders/PostProcessShader.hlsl").string());
		LoadShader<VertexPU>("Skybox_shader", MakePath("shaders/SkyboxShader.hlsl").string());
		LoadShader<VertexPU>("BackBuffer_shader", MakePath("shaders/BackBufferShader.hlsl").string());	
		// IBL shader
		LoadShader<VertexPNUT>("EnvCubeMap_shader", MakePath("shaders/EnvironmentCubeShader.hlsl").string());
		LoadShader<VertexPNUT>("IrradianceMap_shader", MakePath("shaders/IrradianceMapShader.hlsl").string());
		LoadShader<VertexPNUT>("PrefilteredEnvMap_shader", MakePath("shaders/PrefilteredEnvMapShader.hlsl").string());
		LoadShader<VertexPU>("Brdf_shader", MakePath("shaders/BrdfShader.hlsl").string());

		// Debug shader
		LoadShader<VertexPC>("Debug_shader", MakePath("shaders/DebugShader.hlsl").string());

		// Default textures
		LoadTexture("Default_Black", MakePath("resources/textures/Default_Black.png").string(), TextureType::Albedo);
		LoadTexture("Default_White", MakePath("resources/textures/Default_White.png").string(), TextureType::Albedo);
		LoadTexture("Default_Normal", MakePath("resources/textures/Default_Normal.png").string(), TextureType::Normal);
		// Create primitive meshes
		auto quadData = PrimitiveMeshFactory::CreateQuad();
		CreateMesh<VertexPNUT>("Primitive_quad", quadData.vertices, quadData.indices);
		auto cubeData = PrimitiveMeshFactory::CreateCube();
		CreateMesh<VertexPNUT>("Primitive_cube", cubeData.vertices, cubeData.indices);
		auto fullScreenQuadData = PrimitiveMeshFactory::CreateFullScreenQuad();
		CreateMesh<VertexPU>("Primitive_fullscreen_quad", fullScreenQuadData.vertices, fullScreenQuadData.indices);
		auto floorQuadData = PrimitiveMeshFactory::CreateFloorQuad();
		CreateMesh<VertexPNUT>("Primitive_floor_quad", floorQuadData.vertices, floorQuadData.indices);

		// Create materials
		auto defaultMaterial = CreateMaterial("Default_material");
		defaultMaterial->SetShader(GetShader("Default_shader"));
		defaultMaterial->SetRenderState(RenderStateManager::GetInstance().GetState("Opaque"));

		auto	transparentMaterial = CreateMaterial("Transparent_material");
		transparentMaterial->SetShader(GetShader("Forward_Textured_shader"));
		transparentMaterial->SetRenderState(RenderStateManager::GetInstance().GetState("Transparent"));
		transparentMaterial->SetTransparent(true);
		// set default map
		transparentMaterial->SetTexture(0, ResourceManager::GetInstance().GetTexture("Default_White"));	// albedo
		transparentMaterial->SetSampler(0, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));
		transparentMaterial->SetTexture(1, ResourceManager::GetInstance().GetTexture("Default_Normal"));		// normal
		transparentMaterial->SetSampler(1, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));
		transparentMaterial->SetTexture(2, ResourceManager::GetInstance().GetTexture("Default_White"));	// roughness
		transparentMaterial->SetSampler(2, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));
		transparentMaterial->SetTexture(3, ResourceManager::GetInstance().GetTexture("Default_Black"));	// metallic
		transparentMaterial->SetSampler(3, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));
		transparentMaterial->SetTexture(4, ResourceManager::GetInstance().GetTexture("Default_White"));	// ambient occlusion
		transparentMaterial->SetSampler(4, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));
		transparentMaterial->SetTexture(5, ResourceManager::GetInstance().GetTexture("Default_White"));	// opacity
		transparentMaterial->SetSampler(5, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));

		auto texturedMaterial = CreateMaterial("Textured_material");
		texturedMaterial->SetShader(GetShader("Textured_shader"));
		texturedMaterial->SetRenderState(RenderStateManager::GetInstance().GetState("Opaque"));
		// set default map
		texturedMaterial->SetTexture(0, ResourceManager::GetInstance().GetTexture("Default_White"));	// albedo
		texturedMaterial->SetSampler(0, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));
		texturedMaterial->SetTexture(1, ResourceManager::GetInstance().GetTexture("Default_Normal"));		// normal
		texturedMaterial->SetSampler(1, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));
		texturedMaterial->SetTexture(2, ResourceManager::GetInstance().GetTexture("Default_White"));	// roughness
		texturedMaterial->SetSampler(2, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));
		texturedMaterial->SetTexture(3, ResourceManager::GetInstance().GetTexture("Default_Black"));	// metallic
		texturedMaterial->SetSampler(3, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));
		texturedMaterial->SetTexture(4, ResourceManager::GetInstance().GetTexture("Default_White"));	// ambient occlusion
		texturedMaterial->SetSampler(4, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));
		texturedMaterial->SetTexture(5, ResourceManager::GetInstance().GetTexture("Default_White"));	// opacity
		texturedMaterial->SetSampler(5, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));

		// screen billboard material
		auto screenBillboardMaterial = CreateMaterial("Screen_Billboard_material");
		screenBillboardMaterial->SetShader(GetShader("Screen_Billboard_shader"));
		screenBillboardMaterial->SetRenderState(RenderStateManager::GetInstance().GetState("Transparent"));
		screenBillboardMaterial->SetTransparent(true);
		screenBillboardMaterial->SetTexture(0, ResourceManager::GetInstance().GetTexture("Default_White"));
		screenBillboardMaterial->SetSampler(0, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));

		// horizontal billboard material
		auto normalEffectMaterial = CreateMaterial("Normal_Effect_material");
		normalEffectMaterial->SetShader(GetShader("Normal_Effect_shader"));
		normalEffectMaterial->SetRenderState(RenderStateManager::GetInstance().GetState("Transparent"));
		normalEffectMaterial->SetTransparent(true);
		normalEffectMaterial->SetTexture(0, ResourceManager::GetInstance().GetTexture("Default_White"));
		normalEffectMaterial->SetSampler(0, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));
	}

	void ResourceManager::Clear()
	{
		m_models.clear();
		m_textures.clear();
		m_shaders.clear();
		m_meshes.clear();
		m_materials.clear();
		m_fonts.clear();
	}

	std::shared_ptr<Shader> ResourceManager::GetShader(const std::string& name)
	{
		auto it = m_shaders.find(name);
		if (it == m_shaders.end())
		{
			LOG_ERROR("Shader not found: %s", name.c_str());
			return nullptr;
		}

		return it->second;
	}

	std::shared_ptr<Texture2D> ResourceManager::LoadTexture(const std::string& name, const std::string& filePath, TextureType type)
	{
		if (m_textures.find(name) != m_textures.end())
		{
			LOG_WARNING("Texture already exists: %s", name.c_str());
			return m_textures[name];
		}

		auto texture = std::make_shared<Texture2D>();
		if (FAILED(texture->CreateFromFile(filePath, type)))
		{
			LOG_ERROR("Failed to load texture: %s", filePath.c_str());
			return nullptr;
		}
		m_textures[name] = texture;
		LOG_INFO("Texture loaded: %s", filePath.c_str());
		return texture;
	}

	std::shared_ptr<Texture2D> ResourceManager::GetTexture(const std::string& name)
	{
		auto it = m_textures.find(name);
		if (it == m_textures.end())
		{
			LOG_ERROR("Texture not found: %s", name.c_str());
			return nullptr;
		}

		return it->second;
	}

	std::shared_ptr<Model> ResourceManager::LoadModel(const std::string& name, const std::string& filePath)
	{
		if (m_models.find(name) != m_models.end())
		{
			LOG_WARNING("Model already exists: %s", name.c_str());
			return m_models[name];
		}

		auto model = std::make_shared<Model>();
		if (!model->LoadModel(filePath, name))
		{
			LOG_ERROR("Failed to load model: %s", filePath.c_str());
			return nullptr;
		}
		m_models[name] = model;
		LOG_INFO("Model loaded: %s", filePath.c_str());

		return model;
	}

	std::shared_ptr<Model> ResourceManager::GetModel(const std::string& name)
	{
		auto it = m_models.find(name);
		if(it == m_models.end())
		{
			LOG_ERROR("Model not found: %s", name.c_str());
			return nullptr;
		}

		return it->second;
	}

	std::shared_ptr<Animation> ResourceManager::LoadAnimation(const std::string& name, const std::string& filePath, Model* model, bool isLoop)
	{
		if (!model)
		{
			LOG_WARNING("Failed to load Animation: Model is NULL");
			return nullptr;
		}
		if (m_animations.find(name) != m_animations.end())
		{
			LOG_WARNING("Animation already exists: %s", name.c_str());
			return m_animations[name];
		}

		auto animation = std::make_shared<Animation>();
		if (!animation->LoadAnimation(filePath, model, isLoop))
		{
			LOG_ERROR("Failed to load Animation: %s", filePath.c_str());
			return nullptr;
		}
		m_animations[name] = animation;
		LOG_INFO("Animation loaded: %s", filePath.c_str());

		return animation;
	}

	std::shared_ptr<Animation> ResourceManager::GetAnimation(const std::string& name)
	{
		auto it = m_animations.find(name);
		if (it == m_animations.end())
		{
			LOG_ERROR("Animation not found: %s", name.c_str());
			return nullptr;
		}

		return it->second;
	}

	std::shared_ptr<Mesh> ResourceManager::GetMesh(const std::string& name)
	{
		auto it = m_meshes.find(name);
		if (it == m_meshes.end())
		{
			LOG_ERROR("Mess not found: %s", name.c_str());
			return nullptr;
		}

		return it->second;
	}

	std::shared_ptr<Material> ResourceManager::CreateMaterial(const std::string& name)
	{
		if (m_materials.contains(name))
		{
			return m_materials[name];
		}

		auto mat = std::make_shared<Material>();
		mat->SetName(name);

		m_materials[name] = mat;
		return mat;
	}

	std::shared_ptr<Material> ResourceManager::GetMaterial(const std::string& name)
	{
		auto it = m_materials.find(name);
		if (it == m_materials.end())
		{
			LOG_ERROR("Material not found: %s", name.c_str());
			return nullptr;
		}

		return it->second;
	}

	std::shared_ptr<EnvironmentMap> ResourceManager::LoadEnvironmentMap(const std::string& name, const std::string& filePath)
	{
		if (m_environmentMaps.find(name) != m_environmentMaps.end())
		{
			LOG_WARNING("Environment map already exists: %s", name.c_str());
			return m_environmentMaps[name];
		}
		auto envMap = std::make_shared<EnvironmentMap>();
		if (FAILED(envMap->CreateFromHDRFile(filePath)))
		{
			LOG_ERROR("Failed to load environment map: %s", filePath.c_str());
			return nullptr;
		}
		m_environmentMaps[name] = envMap;
		LOG_INFO("Environment map loaded: %s", filePath.c_str());
		return envMap;
	}

	std::shared_ptr<EnvironmentMap> ResourceManager::GetEnvironmentMap(const std::string& name)
	{
		auto it = m_environmentMaps.find(name);
		if (it == m_environmentMaps.end())
		{
			LOG_ERROR("Environment map not found: %s", name.c_str());
			return nullptr;
		}
		return it->second;
	}

	std::shared_ptr<Font> ResourceManager::LoadFont(const std::string& name, const std::string& filePath)
	{
		if (m_fonts.find(name) != m_fonts.end())
		{
			LOG_WARNING("Font already exists: %s", name.c_str());
			return m_fonts[name];
		}
		auto font = std::make_shared<Font>();
		if (!font->Load(filePath))
		{
			LOG_ERROR("Failed to load font : %s", filePath.c_str());
			return nullptr;
		}
		m_fonts[name] = font;
		LOG_INFO("Font loaded: %s", filePath.c_str());
		return font;
	}

	std::shared_ptr<Font> ResourceManager::GetFont(const std::string& name)
	{
		auto it = m_fonts.find(name);
		if (it == m_fonts.end())
		{
			LOG_ERROR("Font not found: %s", name.c_str());
			return nullptr;
		}
		return it->second;
	}
}