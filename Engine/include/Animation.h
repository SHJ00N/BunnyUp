#pragma once

#include <vector>
#include <map>
#include <assimp/scene.h>
#include <functional>

#include "Bone.h"
#include "Animdata.h"

namespace Engine
{
    class Model;

    struct AssimpNodeData 
    {
        Matrix4x4 transformation;
        std::string name;
        int childrenCount = 0;
        std::vector<AssimpNodeData> children;
    };

    class Animation 
    {
    public:
        Animation();
        ~Animation();

        bool LoadAnimation(const std::string& animationPath, Model* model, bool isLooping);
        Bone* FindBone(const std::string& name);

        inline float GetTicksPerSecond() { return m_TicksPerSecond; }
        inline float GetDuration() { return m_Duration; }
        inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
        inline bool IsLooping() { return m_isLooping; }
        inline const std::map<std::string, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }
        inline const Matrix4x4& GetGlobalInverseMatrix() const { return m_GlobalInverseMatrix; }

    private:
        float m_Duration;
        float m_TicksPerSecond;
        bool m_isLooping;
        std::vector<Bone> m_Bones;
        AssimpNodeData m_RootNode;
        std::map<std::string, BoneInfo> m_BoneInfoMap;
        Matrix4x4 m_GlobalInverseMatrix;

        void BuildBoneMapping(const aiAnimation* animation, Model* model);
        void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);
    };
}