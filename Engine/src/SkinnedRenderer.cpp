#include "SkinnedRenderer.h"
#include "ConstantBufferManager.h"
#include "Animator.h"

namespace Engine
{
	void SkinnedRenderer::SetModel(std::shared_ptr<Model> model)
	{
		m_model = model;
		MeshRenderer::SetMesh(model);
	}

	void SkinnedRenderer::UpdateConstantBuffer(ConstantBufferManager& cbManager)
	{
		// set object world matrix
		m_cbPerObject.world = ownerGameObject->transform.GetWorldMatrix();
		m_cbPerObject.normalMatrix = Transpose(Inverse(m_cbPerObject.world));
		cbManager.UpdatePerObject(m_cbPerObject);
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
		cbManager.UpdateSkinPerObject(m_cbSkinPerObject);
	}
}