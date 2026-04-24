#include "SkinnedRenderer.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Animator.h"

namespace Engine
{
	void SkinnedRenderer::SetModel(std::shared_ptr<Model> model)
	{
		m_model = model;
		MeshRenderer::SetMesh(model);
	}

	void SkinnedRenderer::BindConstantBuffer(Renderer& renderer)
	{
		// set object world matrix
		m_cbPerObject.world = ownerGameObject->transform.GetWorldMatrix();
		renderer.UpdatePerObject(m_cbPerObject);
		// set object bone matrix
		auto animator = ownerGameObject->GetComponent<Animator>();
		if (animator)
		{
			auto& bones = animator->GetFinalBoneMatrices();
			for (size_t i = 0; i < bones.size(); ++i)
			{
				m_cbSkinPerObject.bones[i] = bones[i];
			}
		}
		renderer.UpdateSkinPerObject(m_cbSkinPerObject);
	}
}