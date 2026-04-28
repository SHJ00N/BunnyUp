#pragma once

#include "MeshRenderer.h"
#include "ConstantBuffer.h"

namespace Engine
{
	class SkinnedRenderer : public MeshRenderer
	{
	public:
		void SetModel(std::shared_ptr<Model> model);
		Model* GetModel() { return m_model.lock().get(); }
	protected:
		void UpdateConstantBuffer(class Renderer& renderer) override;
	private:
		ConstantBufferSkinPerObject m_cbSkinPerObject;
		std::weak_ptr<Model> m_model;
	};
}