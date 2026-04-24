#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Animdata.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <map>

namespace Engine
{
	class Model
	{
	public:
		Model();
		~Model();

		bool LoadModel(const std::string& path);

		const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return m_meshes; }
		const std::vector<std::shared_ptr<Material>>& GetMaterials() const { return m_materials; }

		int& GetBoneCount() { return m_boneCounter; }
		std::map<std::string, BoneInfo>& GetBoneInfoMap() { return m_boneInfoMap; }

		const Matrix4x4& GetGlobalInverseTransform() const { return m_globalInverseTransform; }

	private:
		std::vector<std::shared_ptr<Mesh>> m_meshes;
		std::vector<std::shared_ptr<Material>> m_materials;
		std::string m_directory;

		// bone data
		Matrix4x4 m_globalInverseTransform;
		std::map<std::string, BoneInfo> m_boneInfoMap;
		int m_boneCounter = 0;

		void processNode(aiNode* node, const aiScene* scene);
		std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
		void loadMaterials(const aiScene* scene);

		// Bone extract funcitons
		void setVertexBoneData(VertexSkin& vertex, int boneID, float weight);
		void extractBoneWeightForVertices(std::vector<VertexSkin>& vertices, aiMesh* mesh, const aiScene* scene);
		void normalizeWeights(std::vector<VertexSkin>& vertices);
	};
}