#include "ResourceManager.h"
#include "Texture2D.h"
#include "Model.h"
#include "Animation.h"

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
		LoadShader<VertexPNUT>("Default", "C:\\Project\\BunnyUp\\Engine\\TextureShader.hlsl");
		LoadShader<VertexSkin>("Skinning", "C:\\Project\\BunnyUp\\Engine\\SkinningShader.hlsl");

		// Load default model
		ResourceManager::GetInstance().LoadModel("Chibi_Rabbit", "C:\\Project\\BunnyUp\\Engine\\resources\\models\\Chibi_Rabbit.fbx");
		ResourceManager::GetInstance().LoadTexture("T_Chibi_Rabbit_03", "C:\\Project\\BunnyUp\\Engine\\resources\\models\\Textures\\T_Chibi_Rabbit_03.png");
		ResourceManager::GetInstance().LoadTexture("T_Chibi_Rabbit_06", "C:\\Project\\BunnyUp\\Engine\\resources\\models\\Textures\\T_Chibi_Rabbit_06.png");
		ResourceManager::GetInstance().LoadTexture("T_Chibi_Emo_25", "C:\\Project\\BunnyUp\\Engine\\resources\\models\\Textures\\T_Chibi_Emo_25.png");
		ResourceManager::GetInstance().LoadTexture("T_Chibi_Emo_21", "C:\\Project\\BunnyUp\\Engine\\resources\\models\\Textures\\T_Chibi_Emo_21.png");
	}

	void ResourceManager::Clear()
	{
		m_models.clear();
		m_textures.clear();
		m_shaders.clear();
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

	std::shared_ptr<Texture2D> ResourceManager::LoadTexture(const std::string& name, const std::string& filePath)
	{
		if (m_textures.find(name) != m_textures.end())
		{
			LOG_WARNING("Texture already exists: %s", name.c_str());
			return m_textures[name];
		}

		auto texture = std::make_shared<Texture2D>();
		if (FAILED(texture->CreateFromFile(filePath)))
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
}