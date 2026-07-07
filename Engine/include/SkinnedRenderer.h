#pragma once

#include "MeshRenderer.h"
#include "ConstantBuffer.h"

namespace Engine
{
	class SkinnedRenderer : public MeshRenderer
	{
	public:
		void SetMesh(std::shared_ptr<Model> model) override;
	protected:
		void UpdateConstantBuffer(class ConstantBufferManager& renderer) override;
	private:
		ConstantBufferSkinPerObject m_cbSkinPerObject;
	};
}