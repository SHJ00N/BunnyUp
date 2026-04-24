#pragma once

#include <vector>
#include <assimp/scene.h>

#include "MathHelper.h"

namespace Engine
{
    struct KeyPosition
    {
        Vector3 position;
        float timeStamp = 0.0f;
    };

    struct KeyRotation
    {
        Quaternion orientation;
        float timeStamp = 0.0f;
    };

    struct KeyScale
    {
        Vector3 scale;
        float timeStamp = 0.0f;
    };

    class Bone
    {
    public:
        Bone(const std::string& name, int ID, const aiNodeAnim* channel);
        void Update(float animationTime);
        Matrix4x4 GetLocalTransform();
        std::string GetBoneName() const;
        int GetBoneID();
        int GetPositionIndex(float animationTime);
        int GetRotationIndex(float animationTime);
        int GetScaleIndex(float animationTime);

        Vector3 GetLocalPosition() const;
        Quaternion GetLocalRotation() const;
        Vector3 GetLocalScale() const;

    private:
        std::vector<KeyPosition> m_Positions;
        std::vector<KeyRotation> m_Rotations;
        std::vector<KeyScale> m_Scales;
        int m_NumPositions;
        int m_NumRotations;
        int m_NumScalings;

        Vector3 m_LocalPosition;
        Quaternion m_LocalRotation;
        Vector3 m_LocalScale;
        Matrix4x4 m_LocalTransform;
        std::string m_Name;
        int m_ID;

        float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
        Vector3 InterpolatePosition(float animationTime);
        Quaternion InterpolateRotation(float animationTime);
        Vector3 InterpolateScaling(float animationTime);
    };
}
