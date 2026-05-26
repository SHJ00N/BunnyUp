#pragma once

#include "RendererComponent.h"
#include "Mesh.h"
#include "Model.h"

namespace Engine
{
	class MeshRenderer : public RendererComponent
	{
	public:
		void SetMesh(std::shared_ptr<Mesh> mesh);
		void SetMesh(std::shared_ptr<Model> model);	// set up meshes and materials with model

		virtual void OnImGui() override;
	protected:
		void UpdateConstantBuffer(ConstantBufferManager& renderer) override;
		void OnRender(ConstantBufferManager& renderer) override;
		void generateBound() override;

	private:
		std::vector<std::shared_ptr<Mesh>> m_meshes;
	};
}