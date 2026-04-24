#pragma once

#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <memory>

#include "MathHelper.h"
#include "Component.h"

namespace Engine
{
    class Animation;
    class Model;
    struct AssimpNodeData;

    const float BLEND_DURATION = 0.15f;

    class Animator : public Component
    {
    public:
        Animator();
        void UpdateAnimation(float dt);
        void PlayAnimation(const std::string& name, bool force = false);
        void PlayAnimation(Animation* pAnimation, bool force = false);
        void CalculateBoneTransform(const AssimpNodeData* node, Matrix4x4 parentTransform);
        void CalculateBoneTransformBlended(const AssimpNodeData* node, Matrix4x4 parentTransform, float weight);
        const std::vector<Matrix4x4>& GetFinalBoneMatrices() const;
        const std::vector<Matrix4x4>& GetGlobalBoneMatrices() const;
        Vector3 GetRootMotionDelta();
        Vector3 ExtractRootMotionDelta(Animation* animation, Vector3& prevRootPos, bool& initalized);
        void RegistAnimation(const std::string& name, Animation* animation);
        Animation* GetAnimation(const std::string& name);
        bool IsAnimationFinished();

        void Update(float dt) override;
        void OnImGui() override;
    protected:
        void OnAwake() override;

    private:
        Model* m_model = nullptr;
        // animation list
        std::map<std::string, Animation*> m_Animations;
        std::vector<Matrix4x4> m_FinalBoneMatrices;

        // for soket
        std::vector<Matrix4x4> m_GlobalBoneMatrices;
        Animation* m_CurrentAnimation = nullptr;
        float m_CurrentTime = 0.0f;
        float m_DeltaTime = 0.0f;

        // blend member
        Animation* m_nextAnimation = nullptr;
        float m_nextTime = 0.0f;
        float m_blendTime = 0.0f;
        float m_blendDuration = BLEND_DURATION;
        bool m_isBlending = false;

        // animtaion position delta
        Vector3 m_prevCurrentRootPosition = Vector3(0.0f, 0.0f, 0.0f);
        Vector3 m_prevNextRootPosition = Vector3(0.0f, 0.0f, 0.0f);
        Vector3 m_rootMotionDelta = Vector3(0.0f, 0.0f, 0.0f);
        bool m_currentRootInitialized = false;
        bool m_nextRootInitialized = false;

        bool m_animationIsFinished = false;
    };
}