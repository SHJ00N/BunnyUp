#include "pch.h"
#include "MeshRenderer.h"
#include "ConstantBufferManager.h"
#include "ImGuiClass.h"
#include "ResourceManager.h"

#include <algorithm>

namespace Engine
{
	void MeshRenderer::SetMesh(std::shared_ptr<Mesh> mesh) 
	{ 
		m_meshes.clear();
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

		generateBound();
	}

	void MeshRenderer::SetMesh(std::shared_ptr<Model> model)
	{
		m_meshes.clear();
		m_meshes = model->GetMeshes();
		
		m_materials.clear();
		for (auto& mat : model->GetMaterials())
		{
			m_materials.push_back(mat->CreateClone());
		}

		generateBound();
	}

	void MeshRenderer::OnRender(ConstantBufferManager& cbManager)
	{
		for (const auto& mesh : m_meshes)
		{
			mesh->Render(m_materials, cbManager);
		}
	}

	void MeshRenderer::UpdateConstantBuffer(ConstantBufferManager& cbManager)
	{
		m_cbPerObject.world = ownerGameObject->transform.GetWorldMatrix();
		m_cbPerObject.normalMatrix = Transpose(Inverse(m_cbPerObject.world));
		cbManager.UpdatePerObject(m_cbPerObject);
	}

	void MeshRenderer::generateBound()
	{
		m_bound.reset();
		m_bound = std::make_unique<AABB>();
		for (const auto& mesh : m_meshes)
		{
			m_bound->MergeBounds(*mesh->GetBound());
		}
	}

	void MeshRenderer::OnImGui()
	{
		for (int m = 0; m < m_materials.size(); ++m)
		{
			auto& mat = m_materials[m];

			std::string matLabel = mat->GetName() + "##" + std::to_string(m);

			if (ImGui::TreeNode(matLabel.c_str()))
			{
				// material color
				auto color = mat->GetColor();
				ImGui::DragFloat3("RGB", &color.x, 0.01f, 0.0f, 1.0f);
				mat->SetColor(Vector4(color.x, color.y, color.z, color.w));

				// material roughness, metallic
				Vector2 mR(mat->GetMetallic(), mat->GetRoughness());
				ImGui::DragFloat2("MR", &mR.x, 0.01f, 0.0f, 1.0f);
				mat->SetMetallic(mR.x);
				mat->SetRoughness(mR.y);

				// material textures
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