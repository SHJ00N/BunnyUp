#pragma once

#include "RendererComponent.h"
#include "Mesh.h"
#include "Model.h"

namespace Engine
{
	class MeshRenderer : public RendererComponent
	{
	public:
		void SetMesh(std::shared_ptr<Mesh> mesh) { m_meshes.push_back(mesh); }
		void SetMesh(std::shared_ptr<Model> model);	// set up meshes and materials with model

		virtual void OnImGui() override;
	protected:
		void BindConstantBuffer(class Renderer& renderer) override;
		void OnRender() override;

	private:
		std::vector<std::shared_ptr<Mesh>> m_meshes;
	};
}