#include "ResourceManager.h"
#include "Texture2D.h"
#include "Model.h"
#include "Animation.h"
#include "PrimitiveMeshFactory.h"

namespace Engine
{
	ResourceManager::ResourceManager()
	{
	}

	ResourceManager::~ResourceManager()
	{
	}

	void ResourceManager::LoadDefaultResources()
	{
		// Load default shader
		LoadShader<VertexPNUT>("Default_shader", "C:\\Project\\BunnyUp\\Engine\\BasicShader.hlsl");
		LoadShader<VertexPNUT>("Textured_shader", "C:\\Project\\BunnyUp\\Engine\\TextureShader.hlsl");
		LoadShader<VertexSkin>("Skinning_shader", "C:\\Project\\BunnyUp\\Engine\\SkinningShader.hlsl");
		LoadShader<VertexPU>("PBR_shader", "C:\\Project\\BunnyUp\\Engine\\PBR_ToonShader.hlsl");
		LoadShader<VertexPU>("PostProcess_shader", "C:\\Project\\BunnyUp\\Engine\\PostProcessShader.hlsl");

		// Create primitive meshes
		auto quadData = PrimitiveMeshFactory::CreateQuad();
		CreateMesh<VertexPNUT>("Primitive_quad", quadData.vertices, quadData.indices);
		auto cubeData = PrimitiveMeshFactory::CreateCube();
		CreateMesh<VertexPNUT>("Primitive_cube", cubeData.vertices, cubeData.indices);
		auto fullScreenQuadData = PrimitiveMeshFactory::CreateFullScreenQuad();
		CreateMesh<VertexPU>("Primitive_fullscreen_quad", fullScreenQuadData.vertices, fullScreenQuadData.indices);

		// Create materials
		auto defaultMaterial = CreateMaterial("Default_material");
		defaultMaterial->SetShader(GetShader("Default_shader"));
		defaultMaterial->SetRenderState(RenderStateManager::GetInstance().GetState("Opaque"));
		defaultMaterial->SetColor(Vector4(0.3f, 0.3f, 0.3f, 1.0f));

		auto texturedMaterial = CreateMaterial("Textured_material");
		texturedMaterial->SetShader(GetShader("Textured_shader"));
		texturedMaterial->SetRenderState(RenderStateManager::GetInstance().GetState("Opaque"));
	}

	void ResourceManager::Clear()
	{
		m_models.clear();
		m_textures.clear();
		m_shaders.clear();
		m_meshes.clear();
		m_materials.clear();
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
		if (!model->LoadModel(filePath))
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
}