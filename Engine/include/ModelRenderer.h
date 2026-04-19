#pragma once

#include "RendererComponent.h"
#include "Mesh.h"
#include "Model.h"

namespace Engine
{
	class ModelRenderer : public RendererComponent
	{
	public:
		void SetModel(std::shared_ptr<Model> model) { m_model = model; }
		virtual void Render(Renderer& renderer) override;
		virtual void OnImGui() override;
	private:
		std::weak_ptr<Model> m_model;
	};
}