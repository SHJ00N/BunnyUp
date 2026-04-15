#pragma once

#include "Mesh.h"
#include "Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Engine
{
	class Model
	{
	public:
		Model();
		~Model();

		bool LoadModel(const std::string& path);
		void Render();
	private:
		std::vector<Mesh> m_meshes;
		std::vector<std::shared_ptr<Material>> m_materials;
		std::string m_directory;

		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		void loadMaterials(const aiScene* scene);
	};
}