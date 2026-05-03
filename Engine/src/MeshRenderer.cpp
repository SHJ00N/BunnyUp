#include "MeshRenderer.h"
#include "Renderer.h"
#include "GameObject.h"
#include "ImGuiClass.h"
#include "ResourceManager.h"

namespace Engine
{
	void MeshRenderer::SetMesh(std::shared_ptr<Mesh> mesh) 
	{ 
		m_meshes.push_back(mesh); 

		// add material to default
		m_materials.resize(mesh->subMeshes.size());
		for (auto& material : m_materials)
		{
			if (!material)
			{
				material = ResourceManager::GetInstance().GetMaterial("Default_material")->CreateClone();
			}
		}
	}

	void MeshRenderer::SetMesh(std::shared_ptr<Model> model)
	{
		m_meshes = model->GetMeshes();
		
		m_materials.clear();
		for (auto& mat : model->GetMaterials())
		{
			m_materials.push_back(mat->CreateClone());
		}
	}

	void MeshRenderer::OnRender(Renderer& renderer)
	{
		for (const auto& mesh : m_meshes)
		{
			mesh->Render(m_materials, renderer);
		}
	}

	void MeshRenderer::UpdateConstantBuffer(Renderer& renderer)
	{
		m_cbPerObject.world = ownerGameObject->transform.GetWorldMatrix();
		renderer.UpdatePerObject(m_cbPerObject);
	}

	void MeshRenderer::OnImGui()
	{
		for (int m = 0; m < m_materials.size(); ++m)
		{
			auto& mat = m_materials[m];

			auto color = mat->GetColor();
			ImGui::DragFloat4("Color", &color.x, 0.01f);
			mat->SetColor(Vector4(color.x, color.y, color.z, color.w));

			std::string matLabel = mat->GetName() + "##" + std::to_string(m);

			if (ImGui::TreeNode(matLabel.c_str()))
			{
				auto& textures = mat->GetTextures();

				for (int i = 0; i < textures.size(); ++i)
				{
					std::string texLabel = "Texture Slot " + std::to_string(i);

					std::string currentName = textures[i] ? textures[i]->GetName() : "None";

					ImGui::Text("%s:", texLabel.c_str());
					ImGui::SameLine();

					ImGui::SetNextItemWidth(200.0f); // 원하는 값
					std::string comboLabel = "Texture##" + std::to_string(i);

					if (ImGui::BeginCombo(comboLabel.c_str(), currentName.c_str()))
					{
						const auto& textures = ResourceManager::GetInstance().GetTextures();

						for (auto& tex : textures)
						{
							bool selected = (tex.second == mat->GetTexture(i));

							if (ImGui::Selectable(tex.second->GetName().c_str(), selected))
							{
								mat->SetTexture(i, tex.second);
							}

							if (selected)
								ImGui::SetItemDefaultFocus();
						}

						ImGui::EndCombo();
					}
				}

				ImGui::TreePop();
			}
		}
	}
}