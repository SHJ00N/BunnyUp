#include "Animator.h"
#include "Animation.h"
#include "SkinnedRenderer.h"
#include "GameObject.h"

#include "imgui.h"

namespace Engine
{
    Animator::Animator() 
    {
    }

#pragma region override
    void Animator::OnAwake()
    {
        auto renderer = ownerGameObject->GetComponent<SkinnedRenderer>();
        if (renderer)
        {
            m_model = renderer->GetModel();
        }

        m_CurrentTime = 0.0f;
        m_CurrentAnimation = nullptr;

        m_FinalBoneMatrices.resize(256);
        m_GlobalBoneMatrices.resize(256);

        for (int i = 0; i < 256; i++)
        {
            m_FinalBoneMatrices[i] = Matrix4x4(1.0f);
            m_GlobalBoneMatrices[i] = Matrix4x4(1.0f);
        }
    }

    void Animator::Update(float dt)
    {
        UpdateAnimation(dt);
    }

    void Animator::OnImGui()
    {
        for (const auto& animation : m_Animations)
        {
            bool isSelected = animation.second == m_CurrentAnimation;

            if (ImGui::Selectable(animation.first.c_str(), isSelected))
            {
                PlayAnimation(animation.first);
            }
        }
    }
#pragma endregion

    void Animator::RegistAnimation(const std::string& name, Animation* animation)
    {
        m_Animations[name] = animation;
    }

    Animation* Animator::GetAnimation(const std::string& name)
    {
        if (m_Animations.find(name) != m_Animations.end())
        {
            return m_Animations[name];
        }
        return nullptr;
    }

    bool Animator::IsAnimationFinished()
    {
        bool isFinished = m_animationIsFinished;
        m_animationIsFinished = false; // reset the flag after checking
        return isFinished;
    }

    void Animator::UpdateAnimation(float dt) {
        if (!m_CurrentAnimation) return;

        float prevTime = m_CurrentTime;
        // update current animation time
        m_DeltaTime = dt;
        m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;

        if (!m_CurrentAnimation->IsLooping())
        {
            if (m_CurrentTime >= m_CurrentAnimation->GetDuration())
            {
                m_CurrentTime = min(m_CurrentTime, m_CurrentAnimation->GetDuration() - 0.0001f);
                m_animationIsFinished = true;
            }
            else
            {
                m_animationIsFinished = false;
            }
        }
        else
        {
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
            m_animationIsFinished = (m_CurrentTime < prevTime); // animation looped
        }

        // reset root motion delta on animation loop
        if (m_CurrentAnimation->IsLooping() && m_CurrentTime < prevTime)
        {
            m_currentRootInitialized = false;
        }

        // if request animation change, animation blending perform
        if (m_isBlending)
        {
            // update next animation time
            m_nextTime += m_nextAnimation->GetTicksPerSecond() * dt;
            m_nextTime = fmod(m_nextTime, m_nextAnimation->GetDuration());
            // calculate blend factor
            m_blendTime += dt;
            float weight = std::clamp(m_blendTime / m_blendDuration, 0.0f, 1.0f);

            // end blending
            if (weight >= 1.0f)
            {
                m_CurrentAnimation = m_nextAnimation;
                m_CurrentTime = m_nextTime;
                m_nextAnimation = nullptr;
                m_isBlending = false;

                CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), Matrix4x4(1.0f));
                return;
            }

            // blending
            CalculateBoneTransformBlended(&m_CurrentAnimation->GetRootNode(), Matrix4x4(1.0f), weight);

            // calculate delta
            m_rootMotionDelta = ExtractRootMotionDelta(m_nextAnimation, m_prevNextRootPosition, m_nextRootInitialized);
        }
        else
        {
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), Matrix4x4(1.0f));
            m_rootMotionDelta = ExtractRootMotionDelta(m_CurrentAnimation, m_prevCurrentRootPosition, m_currentRootInitialized);
        }
    }

    void Animator::PlayAnimation(const std::string& name, bool force)
    {
        PlayAnimation(m_Animations[name], force);
    }

    void Animator::PlayAnimation(Animation* pAnimation, bool force) {
        if (!pAnimation || !m_model) return;

        // first call
        if (!m_CurrentAnimation)
        {
            m_CurrentAnimation = pAnimation;
            m_CurrentTime = 0.0f;
            m_currentRootInitialized = false;
            return;
        }

        // already playing the same animation
        if (m_CurrentAnimation == pAnimation && !force && !m_animationIsFinished) return;

        // if force play, immediately switch to new animation without blending
        if (force)
        {
            m_isBlending = false;
            m_nextAnimation = nullptr;

            m_CurrentAnimation = pAnimation;
            m_CurrentTime = 0.0f;
            m_currentRootInitialized = false;
            return;
        }

        // animation blending setup
        if (m_CurrentAnimation != pAnimation && m_nextAnimation != pAnimation)
        {
            m_nextAnimation = pAnimation;
            m_nextTime = 0.0f;
            m_blendTime = 0.0f;
            m_nextRootInitialized = false;
            m_isBlending = true;
        }
    }

    Vector3 Animator::ExtractRootMotionDelta(Animation* animation, Vector3& prevRootPos, bool& initalized)
    {
        Bone* bone = animation->FindBone("root_$AssimpFbx$_Translation");
        if (!bone) return Vector3(0.0f, 0.0f, 0.0f);

        Vector3 position = bone->GetLocalPosition();
        // first frame, initialize previous root position
        if (!initalized)
        {
            prevRootPos = position;
            initalized = true;
            return Vector3(0.0f, 0.0f, 0.0f);
        }

        // calculate delta
        Vector3 delta = position - prevRootPos;
        prevRootPos = position;

        return Vector3(delta.x, 0.0f, delta.z);
    }

    Vector3 Animator::GetRootMotionDelta()
    {
        Vector3 delta = m_rootMotionDelta;
        m_rootMotionDelta = Vector3(0.0f, 0.0f, 0.0f);
        return delta;
    }

    void Animator::CalculateBoneTransform(const AssimpNodeData* node, Matrix4x4 parentTransform) {
        std::string nodeName = node->name;
        Matrix4x4 nodeTransform = node->transformation;

        Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

        if (Bone) {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }

        Matrix4x4 globalTransformation = nodeTransform * parentTransform;

        auto boneInfoMap = m_model->GetBoneInfoMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) 
        {
            int index = boneInfoMap[nodeName].id;
            auto offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = offset * globalTransformation;

            // for soket
            m_GlobalBoneMatrices[index] = globalTransformation;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }

    void Animator::CalculateBoneTransformBlended(const AssimpNodeData* node, Matrix4x4 parentTransform, float weight)
    {
        std::string nodeName = node->name;
        Matrix4x4 nodeTransform = node->transformation;
        // bone data
        Bone* curBone = m_CurrentAnimation->FindBone(nodeName);
        Bone* nextBone = m_nextAnimation->FindBone(nodeName);
        if (curBone) curBone->Update(m_CurrentTime);
        if (nextBone) nextBone->Update(m_nextTime);

        if (curBone && nextBone)
        {
            // blended local transform
           Vector3 position = Lerp(curBone->GetLocalPosition(), nextBone->GetLocalPosition(), weight);
            Quaternion rotation = Slerp(curBone->GetLocalRotation(), nextBone->GetLocalRotation(), weight);
            Vector3 scale = Lerp(curBone->GetLocalScale(), nextBone->GetLocalScale(), weight);

            nodeTransform = Scaling(scale) * Rotation(rotation) * Translation(position);
        }
        else if (curBone)
        {
            nodeTransform = curBone->GetLocalTransform();
        }
        else if (nextBone)
        {
            nodeTransform = nextBone->GetLocalTransform();
        }

        Matrix4x4 globalBlendedTransformation = nodeTransform * parentTransform;

        auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int index = boneInfoMap[nodeName].id;
            auto offset = boneInfoMap[nodeName].offset;

            m_FinalBoneMatrices[index] = offset * globalBlendedTransformation;

            // for soket
            m_GlobalBoneMatrices[index] = globalBlendedTransformation;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransformBlended(&node->children[i], globalBlendedTransformation, weight);
    }

    const std::vector<Matrix4x4>& Animator::GetFinalBoneMatrices() const
    {
        return m_FinalBoneMatrices;
    }

    const std::vector<Matrix4x4>& Animator::GetGlobalBoneMatrices() const
    {
        return m_GlobalBoneMatrices;
    }
}