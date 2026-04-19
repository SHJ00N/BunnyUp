#include "ModelRenderer.h"
#include "Renderer.h"
#include "GameObject.h"
#include "ImGuiClass.h"

namespace Engine
{
	void ModelRenderer::Render(Renderer& renderer)
	{
		renderer.SetWorldMatrix(Transpose(ownerGameObject->transform.GetWorldMatrix()));
		renderer.UpdateConstantBuffer();

		auto modelPtr = m_model.lock();
		if (modelPtr)
		{
			auto& materials = modelPtr->GetMaterials();
			auto& meshes = modelPtr->GetMeshes();

			for (auto& mesh : meshes)
			{
				mesh.Render(materials);
			}
		}
	}

	void ModelRenderer::OnImGui()
	{
		auto modelPtr = m_model.lock();
		if (modelPtr)
		{
			ImGui::Text("Model: %s", modelPtr->GetMeshes().size() > 0 ? "Loaded" : "None");
		}
		else
		{
			ImGui::Text("Model: None");
		}
	}
}