#include "Animation.h"
#include "Model.h"
#include "Log.h"
#include "Assimp_DX_helpers.h"

namespace Engine
{
    Animation::Animation() : m_isLooping(false)
    {
    }

    Animation::~Animation()
    {
    }

    bool Animation::LoadAnimation(const std::string& animationPath, Engine::Model* model, bool isLooping)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
        if (!scene || !scene->mRootNode)
        {
            return false;
        }

        auto animation = scene->mAnimations[0];
        m_Duration = static_cast<float>(animation->mDuration);
        m_TicksPerSecond = static_cast<float>(animation->mTicksPerSecond);

        aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
        globalTransformation = globalTransformation.Inverse();
        m_GlobalInverseMatrix = AssimpDXHelpers::ConvertMatrixToDXFormat(globalTransformation);

        BuildBoneMapping(animation, model);
        ReadHierarchyData(m_RootNode, scene->mRootNode);

        m_isLooping = isLooping;

        return true;
    }

    Bone* Animation::FindBone(const std::string& name) {
        auto it = m_BoneMap.find(name);
        if (it != m_BoneMap.end())
        {
            return it->second;
        }
        return nullptr;
    }

    void Animation::BuildBoneMapping(const aiAnimation* animation, Model* model) {
        int size = animation->mNumChannels;
        auto& boneInfoMap = model->GetBoneInfoMap();
        m_Bones.reserve(size);

        for (int i = 0; i < size; i++)
        {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;
            
            int id = -1;
            if (boneInfoMap.find(boneName) != boneInfoMap.end())
            {
                id = boneInfoMap[boneName].id;
            }

            // add new bone
            m_Bones.emplace_back(boneName, id, channel);
            m_BoneMap[boneName] = &m_Bones.back();  // set bone pointer map
        }
    }

    void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src) {
        assert(src);

        std::string name = src->mName.data;
        dest.name = name;
        // set node's bone pointer
        if (m_BoneMap.find(name) != m_BoneMap.end())
        {
            dest.bone = m_BoneMap[name];
        }
        dest.transformation = AssimpDXHelpers::ConvertMatrixToDXFormat(src->mTransformation);
        dest.childrenCount = src->mNumChildren;

        for (unsigned int i = 0; i < src->mNumChildren; i++) {
            AssimpNodeData newData;
            ReadHierarchyData(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }
}