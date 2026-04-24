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

        ReadHierarchyData(m_RootNode, scene->mRootNode);
        BuildBoneMapping(animation, model);

        m_isLooping = isLooping;

        return true;
    }

    Bone* Animation::FindBone(const std::string& name) {
        auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
            [&](const Bone& Bone) { return Bone.GetBoneName() == name; });
        if (iter == m_Bones.end()) return nullptr;
        else return &(*iter);
    }

    void Animation::BuildBoneMapping(const aiAnimation* animation, Model* model) {
        int size = animation->mNumChannels;

        auto& boneInfoMap = model->GetBoneInfoMap();
        int& boneCount = model->GetBoneCount();

        for (int i = 0; i < size; i++)
        {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;
            
            int id = -1;
            if (boneInfoMap.find(boneName) != boneInfoMap.end())
            {
                id = boneInfoMap[boneName].id;
            }

            m_Bones.push_back(Bone(channel->mNodeName.data, id, channel));
        }

        m_BoneInfoMap = boneInfoMap;
    }

    void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src) {
        assert(src);

        dest.name = src->mName.data;
        dest.transformation = AssimpDXHelpers::ConvertMatrixToDXFormat(src->mTransformation);
        dest.childrenCount = src->mNumChildren;

        for (unsigned int i = 0; i < src->mNumChildren; i++) {
            AssimpNodeData newData;
            ReadHierarchyData(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }
}