#include "Model.h"
#include "Texture2D.h"
#include "RenderStateManager.h"
#include "SamplerStateManager.h"
#include "ResourceManager.h"
#include "Log.h"

namespace Engine
{
	Model::Model()
	{
	}

	Model::~Model()
	{
	}

	bool Model::LoadModel(const std::string& path)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path,
			aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
		if (!scene || !scene->mRootNode)
		{
			return false;
		}

		// save path
		m_directory = path.substr(0, path.find_last_of('/'));
		// create mesh and materials
		loadMaterials(scene);
		processNode(scene->mRootNode, scene);

		return true;
	}

	void Model::Render()
	{
		for (auto& mesh : m_meshes)
		{
			mesh.Render(m_materials);
		}
	}

	void Model::processNode(aiNode* node, const aiScene* scene) 
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_meshes.push_back(processMesh(mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}

    Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) 
	{
		Mesh newMesh;

		std::vector<VertexPT> vertices;
		std::vector<uint32_t> indices;

		// Create vertex
		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{
			VertexPT v;

			v.position = {
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z
			};

			if (mesh->mTextureCoords[0])
			{
				v.texCoord = {
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y
				};
			}

			vertices.push_back(v);
		}

		// Create indices
		for (UINT i = 0; i < mesh->mNumFaces; i++)
		{
			const aiFace& face = mesh->mFaces[i];

			for (UINT j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		// Create mesh(gpu buffers)
		newMesh.CreateMesh<VertexPT>(vertices, indices);

		// add sub mesh
		SubMesh sub;
		sub.indexStart = 0;
		sub.indexCount = static_cast<uint32_t>(indices.size());
		sub.materialIndex = mesh->mMaterialIndex;

		newMesh.subMeshes.push_back(sub);

		return newMesh;
    }

	void Model::loadMaterials(const aiScene* scene)
	{
		m_materials.resize(scene->mNumMaterials);

		for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
		{
			aiMaterial* aiMat = scene->mMaterials[i];
			auto mat = std::make_shared<Material>();

			// set material name
			aiString name;
			if (aiMat->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
			{
				mat->SetName(name.C_Str());
			}
			else
			{
				mat->SetName("Material_" + std::to_string(i));
			}

			// create material textures
			aiString path;
			if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				std::string fullPath = m_directory + '/' + path.C_Str();
				// load texture
				auto tex = ResourceManager::GetInstance().LoadTexture(mat->GetName() + "_Diffuse", fullPath);
				// set texture slot
				mat->SetTexture(0, tex);
				// set sampler slot
				mat->SetSampler(0, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));
			}

			// set shader
			mat->SetShader(ResourceManager::GetInstance().GetShader("Default"));
			// set render state
			mat->SetRenderState(RenderStateManager::GetInstance().GetState("Opaque"));

			m_materials[i] = mat;
		}
	}
}