#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "Singleton.h"
#include "Shader.h"
#include "Log.h"
#include "Model.h"

namespace Engine
{
	class Shader;
	class Texture2D;
	class Animation;
	class EnvironmentMap;
	class Font;

	class ResourceManager : public Singleton<ResourceManager>
	{
	public:
		ResourceManager();
		~ResourceManager();

		void LoadDefaultResources();

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
		std::shared_ptr<Shader> GetShader(const std::string& name);
		const std::unordered_map<std::string, std::shared_ptr<Shader>>& GetShaders() const { return m_shaders; }

		std::shared_ptr<Texture2D> LoadTexture(const std::string& name, const std::string& filePath, TextureType type = TextureType::Default);
		std::shared_ptr<Texture2D> GetTexture(const std::string& name);
		const std::unordered_map<std::string, std::shared_ptr<Texture2D>>& GetTextures() const { return m_textures; }

		std::shared_ptr<Model> LoadModel(const std::string& name, const std::string& filePath);
		std::shared_ptr<Model> GetModel(const std::string& name);
		const std::unordered_map<std::string, std::shared_ptr<Model>>& GetModels() const { return m_models; }

		std::shared_ptr<Animation> LoadAnimation(const std::string& name, const std::string& filePath, Model* model, bool isLoop = false);
		std::shared_ptr<Animation> GetAnimation(const std::string& name);
		const std::unordered_map<std::string, std::shared_ptr<Animation>>& GetAnimations() const { return m_animations; }

		std::shared_ptr<EnvironmentMap> LoadEnvironmentMap(const std::string& name, const std::string& filePath);
		std::shared_ptr<EnvironmentMap> GetEnvironmentMap(const std::string& name);
		const std::unordered_map<std::string, std::shared_ptr<EnvironmentMap>>& GetEnvironmentMaps() const { return m_environmentMaps; }

		std::shared_ptr<Font> LoadFont(const std::string& name, const std::string& filePath);
		std::shared_ptr<Font> GetFont(const std::string& name);
		const std::unordered_map<std::string, std::shared_ptr<Font>>& GetFonts() const { return m_fonts; }

		// Mesh for caching (e.g. primitive)
		template<typename VertexType>
		std::shared_ptr<Mesh> CreateMesh(const std::string& name, const std::vector<VertexType>& vertices, const std::vector<uint32_t>& indices)
		{
			if (m_meshes.contains(name))
			{
				return m_meshes[name];
			}

			// Create new mesh
			auto mesh = std::make_shared<Mesh>();
			mesh->CreateMesh<VertexType>(vertices, indices);

			// add sub mesh
			SubMesh sub;
			sub.indexStart = 0;
			sub.indexCount = static_cast<uint32_t>(indices.size());
			sub.materialIndex = 0;
			mesh->subMeshes.push_back(sub);

			m_meshes[name] = mesh;
			return mesh;
		}
		// Non-cached meshes(e.g. model meshes)
		template<typename VertexType>
		std::shared_ptr<Mesh> CreateMesh(const std::vector<VertexType>& vertices, const std::vector<uint32_t>& indices)
		{
			auto mesh = std::make_shared<Mesh>();

			if (!mesh->CreateMesh(vertices, indices))
				return nullptr;

			return mesh;
		}
		std::shared_ptr<Mesh> GetMesh(const std::string& name);
		const std::unordered_map<std::string, std::shared_ptr<Mesh>>& GetMeshes() const { return m_meshes; }

		std::shared_ptr<Material> CreateMaterial(const std::string& name);
		std::shared_ptr<Material> GetMaterial(const std::string& name);
		const std::unordered_map<std::string, std::shared_ptr<Material>>& GetMaterials() const { return m_materials; }

		void Clear();
	private:
		// resource storage
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_textures;
		std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
		std::unordered_map<std::string, std::shared_ptr<Animation>> m_animations;
		std::unordered_map<std::string, std::shared_ptr<Mesh>> m_meshes;	// primitive meshes
		std::unordered_map<std::string, std::shared_ptr<Material>> m_materials;
		std::unordered_map<std::string, std::shared_ptr<EnvironmentMap>> m_environmentMaps;
		std::unordered_map<std::string, std::shared_ptr<Font>> m_fonts;
	};
}