#include "Model.h"
#include "Texture2D.h"
#include "RenderStateManager.h"
#include "SamplerStateManager.h"
#include "ResourceManager.h"
#include "Log.h"
#include "Assimp_DX_helpers.h"

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
		processNode(scene->mRootNode, scene);
		loadMaterials(scene);

		// Set model inverse root
		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
		globalTransformation = globalTransformation.Inverse();
		m_globalInverseTransform = AssimpDXHelpers::ConvertMatrixToDXFormat(globalTransformation);

		return true;
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

    std::shared_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene) 
	{
		std::vector<VertexPNUT> vertices;
		std::vector<uint32_t> indices;

		// Create vertex
		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{
			VertexPNUT v;

			v.position = {
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z
			};

			if (mesh->HasNormals())
			{
				v.normal = {
					mesh->mNormals[i].x,
					mesh->mNormals[i].y,
					mesh->mNormals[i].z
				};
			}
			else
			{
				v.normal = { 0.0, 1.0f, 0.0f };
			}

			if (mesh->mTextureCoords[0])
			{
				v.uv = {
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y
				};
			}

			if (mesh->HasTangentsAndBitangents())
			{
				Vector3 T = {
						mesh->mTangents[i].x,
						mesh->mTangents[i].y,
						mesh->mTangents[i].z
				};

				Vector3 B = {
					mesh->mBitangents[i].x,
					mesh->mBitangents[i].y,
					mesh->mBitangents[i].z
				};

				Vector3 N = {
					mesh->mNormals[i].x,
					mesh->mNormals[i].y,
					mesh->mNormals[i].z
				};

				T = Normalize(T);
				B = Normalize(B);
				N = Normalize(N);

				float handedness = (Dot(Cross(N, T), B) < 0.0f) ? -1.0f : 1.0f;

				v.tangent = { T.x, T.y, T.z, handedness };
			}
			else
			{
				v.tangent = { 1.0f, 0.0f, 0.0f, 1.0f };
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
		auto newMesh = std::make_shared<Mesh>();
		if (mesh->HasBones())
		{
			// Create skinned vertices
			std::vector<VertexSkin> skinnedVertices(vertices.size());
			for (size_t i = 0; i < vertices.size(); ++i)
			{
				skinnedVertices[i] = { vertices[i].position, vertices[i].normal, vertices[i].uv, vertices[i].tangent };
				for (int j = 0; j < MaxWeight; ++j) {
					skinnedVertices[i].boneIDs[j] = -1;
					skinnedVertices[i].weights[j] = 0.0f;
				}
			}

			// Setup vertices bone data
			extractBoneWeightForVertices(skinnedVertices, mesh, scene);
			normalizeWeights(skinnedVertices);

			// Create skinned mesh
			newMesh->CreateMesh<VertexSkin>(skinnedVertices, indices);
			newMesh->SetSkinned(true);
		}
		else
		{
			// Create static mesh
			newMesh->CreateMesh<VertexPNUT>(vertices, indices);
		}

		// add sub mesh
		SubMesh sub;
		sub.indexStart = 0;
		sub.indexCount = static_cast<uint32_t>(indices.size());
		sub.materialIndex = mesh->mMaterialIndex;

		newMesh->subMeshes.push_back(sub);

		return newMesh;
    }

	void Model::loadMaterials(const aiScene* scene)
	{
		m_materials.resize(scene->mNumMaterials);

		for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
		{
			aiMaterial* aiMat = scene->mMaterials[i];
			auto mat = std::make_unique<Material>();

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
			mat->SetShader(m_boneCounter ? ResourceManager::GetInstance().GetShader("Skinning") : ResourceManager::GetInstance().GetShader("Default"));
			// set render state
			mat->SetRenderState(RenderStateManager::GetInstance().GetState("Opaque"));

			m_materials[i] = std::move(mat);
		}
	}

	void Model::extractBoneWeightForVertices(std::vector<VertexSkin>& vertices, aiMesh* mesh, const aiScene* scene) 
	{
		auto& boneInfoMap = m_boneInfoMap;
		int& boneCount = m_boneCounter;

		for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) 
		{
			int boneID = -1;
			std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				BoneInfo newBoneInfo;
				newBoneInfo.id = boneCount;
				// setup bone offset data
				newBoneInfo.offset = AssimpDXHelpers::ConvertMatrixToDXFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
				boneInfoMap[boneName] = newBoneInfo;
				boneID = boneCount;
				boneCount++;
			}
			else 
			{
				boneID = boneInfoMap[boneName].id;
			}

			assert(boneID != -1);
			auto weights = mesh->mBones[boneIndex]->mWeights;
			int numWeights = mesh->mBones[boneIndex]->mNumWeights;

			for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
			{
				int vertexId = weights[weightIndex].mVertexId;
				float weight = weights[weightIndex].mWeight;
				assert(vertexId < vertices.size());
				setVertexBoneData(vertices[vertexId], boneID, weight);
			}
		}
	}

	void Model::setVertexBoneData(VertexSkin& vertex, int boneID, float weight) 
	{
		for (int i = 0; i < MaxWeight; i++)
		{
			if (vertex.boneIDs[i] < 0) 
			{
				vertex.boneIDs[i] = boneID;
				vertex.weights[i] = weight;
				break;
			}
		}
	}

	void Model::normalizeWeights(std::vector<VertexSkin>& vertices)
	{
		for (auto& v : vertices)
		{
			float sum = 0.0f;

			for (int i = 0; i < MaxWeight; i++)
				sum += v.weights[i];

			if (sum > 0.0f)
			{
				for (int i = 0; i < MaxWeight; i++)
					v.weights[i] /= sum;
			}
			else
			{
				v.weights[0] = 1.0f;
				v.boneIDs[0] = 0;
			}
		}
	}
}