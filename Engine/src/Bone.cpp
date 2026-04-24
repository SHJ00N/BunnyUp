#include "Bone.h"
#include "Assimp_DX_helpers.h"

namespace Engine
{
    Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
        : m_Name(name), m_ID(ID), m_LocalTransform(Matrix4x4(1.0f)), m_LocalPosition(Vector3(0.0, 0.0, 0.0f)), m_LocalRotation(Quaternion::Identity()), m_LocalScale(Vector3(1.0f, 1.0f, 1.0f)) 
    {
        m_NumPositions = channel->mNumPositionKeys;

        for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex) {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float timeStamp = static_cast<float>(channel->mPositionKeys[positionIndex].mTime);
            KeyPosition data;
            data.position = AssimpDXHelpers::GetDXVec(aiPosition);
            data.timeStamp = timeStamp;
            m_Positions.push_back(data);
        }

        m_NumRotations = channel->mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex) {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float timeStamp = static_cast<float>(channel->mRotationKeys[rotationIndex].mTime);
            KeyRotation data;
            data.orientation = AssimpDXHelpers::GetDXQuat(aiOrientation);
            data.timeStamp = timeStamp;
            m_Rotations.push_back(data);
        }

        m_NumScalings = channel->mNumScalingKeys;
        for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex) {
            aiVector3D aiScale = channel->mScalingKeys[keyIndex].mValue;
            float timeStamp = static_cast<float>(channel->mScalingKeys[keyIndex].mTime);
            KeyScale data;
            data.scale = AssimpDXHelpers::GetDXVec(aiScale);
            data.timeStamp = timeStamp;
            m_Scales.push_back(data);
        }
    }

    void Bone::Update(float animationTime)
    {
        m_LocalPosition = InterpolatePosition(animationTime);
        m_LocalRotation = InterpolateRotation(animationTime);
        m_LocalScale = InterpolateScaling(animationTime);

        m_LocalTransform = Scaling(m_LocalScale) * Rotation(m_LocalRotation) * Translation(m_LocalPosition);
    }

    Vector3 Bone::GetLocalPosition() const 
    {
        return m_LocalPosition;
    }

    Quaternion Bone::GetLocalRotation() const 
    {
        return m_LocalRotation;
    }

    Vector3 Bone::GetLocalScale() const 
    {
        return m_LocalScale;
    }

    Matrix4x4 Bone::GetLocalTransform() 
    {
        return m_LocalTransform;
    }

    std::string Bone::GetBoneName() const
    {
        return m_Name;
    }

    int Bone::GetBoneID() {
        return m_ID;
    }

    int Bone::GetPositionIndex(float animationTime) {
        for (int index = 0; index < m_NumPositions - 1; ++index) {
            if (animationTime < m_Positions[index + 1].timeStamp) return index;
        }
        return m_NumPositions - 1;
    }

    int Bone::GetRotationIndex(float animationTime) {
        for (int index = 0; index < m_NumRotations - 1; ++index) {
            if (animationTime < m_Rotations[index + 1].timeStamp) return index;
        }
        return m_NumRotations - 1;
    }

    int Bone::GetScaleIndex(float animationTime) {
        for (int index = 0; index < m_NumScalings - 1; ++index) {
            if (animationTime < m_Scales[index + 1].timeStamp) return index;
        }
        return m_NumScalings - 1;
    }

    float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
        float scaleFactor = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float frameDiff = nextTimeStamp - lastTimeStamp;
        scaleFactor = midWayLength / frameDiff;
        return scaleFactor;
    }

    Vector3 Bone::InterpolatePosition(float animationTime) {
        if (1 == m_NumPositions) return m_Positions[0].position;

        int p0Index = GetPositionIndex(animationTime);
        int p1Index = p0Index + 1;

        if (p1Index >= m_NumPositions)
            p1Index = m_NumPositions - 1;

        float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp, m_Positions[p1Index].timeStamp, animationTime);
        Vector3 finalPosition = Lerp(m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);
        return finalPosition;
    }

    Quaternion Bone::InterpolateRotation(float animationTime) {
        if (1 == m_NumRotations) {
            auto rotation = Normalize(m_Rotations[0].orientation);
            return rotation;
        }

        int p0Index = GetRotationIndex(animationTime);
        int p1Index = p0Index + 1;

        if (p1Index >= m_NumRotations)
            p1Index = m_NumRotations - 1;

        float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp, m_Rotations[p1Index].timeStamp, animationTime);
        Quaternion finalRotation = Slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, scaleFactor);
        finalRotation = Normalize(finalRotation);
        return finalRotation;
    }

    Vector3 Bone::InterpolateScaling(float animationTime) {
        if (1 == m_NumScalings) return m_Scales[0].scale;

        int p0Index = GetScaleIndex(animationTime);
        int p1Index = p0Index + 1;

        if (p1Index >= m_NumScalings)
            p1Index = m_NumScalings - 1;

        float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp, m_Scales[p1Index].timeStamp, animationTime);
        Vector3 finalScale = Lerp(m_Scales[p0Index].scale, m_Scales[p1Index].scale, scaleFactor);
        return finalScale;
    }
}