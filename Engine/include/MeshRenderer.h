#pragma once

#include "RendererComponent.h"
#include "Mesh.h"
#include "Model.h"

namespace Engine
{
	class MeshRenderer : public RendererComponent
	{
	public:
		MeshRenderer();
		virtual ~MeshRenderer() = default;

		void SetMesh(std::shared_ptr<Mesh> mesh);
		virtual void SetMesh(std::shared_ptr<Model> model);	// set up meshes and materials with model

		Model* GetModel() { return m_model.get(); }

		virtual void OnImGui() override;
	protected:
		void UpdateConstantBuffer(ConstantBufferManager& renderer) override;
		void OnRender(ConstantBufferManager& renderer) override;
		void generateBound() override;
		
	private:
		std::vector<std::shared_ptr<Mesh>> m_meshes;
		std::shared_ptr<Model> m_model;
	};
}