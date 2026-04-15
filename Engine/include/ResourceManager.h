#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "Singleton.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Model.h"
#include "Log.h"

namespace Engine
{
	class ResourceManager : public Singleton<ResourceManager>
	{
	public:
		ResourceManager();
		~ResourceManager();

		template<typename VertexType>
		std::shared_ptr<Shader> LoadShader(const std::string& name, const std::string& filePath)
		{
			if(m_shaders.find(name) != m_shaders.end())
			{
				LOG_WARNING("Shader already exists: %s", name.c_str());
				return m_shaders[name];
			}

			auto shader = std::make_shared<Shader>();
			if (FAILED(shader->CompileFromFile<VertexType>(filePath)))
			{
				LOG_ERROR("Failed to load shader: %s", filePath.c_str());
				return nullptr;
			}
			m_shaders[name] = shader;
			LOG_INFO("Shader loaded: %s", filePath.c_str());
			return shader;
		}
		std::shared_ptr<Shader> GetShader(const std::string& name) const;

		std::shared_ptr<Texture2D> LoadTexture(const std::string& name, const std::string& filePath);
		std::shared_ptr<Texture2D> GetTexture(const std::string& name) const;

		std::shared_ptr<Model> LoadModel(const std::string& name, const std::string& filePath);
		std::shared_ptr<Model> GetModel(const std::string& name) const;

		void Clear();
	private:
		// resource storage
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_textures;
		std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
	};
}