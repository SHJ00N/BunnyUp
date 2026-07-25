#include "pch.h"
#include "RendererComponent.h"
#include "ImGuiClass.h"
#include "ResourceManager.h"

namespace Engine
{
	// Set all material to parameter
	void RendererComponent::SetMaterial(std::shared_ptr<Material> material)
	{
		for (auto& mat : m_materials)
		{
			mat = material;
		}
	}
	// Set material corresponded to index
	void RendererComponent::SetMaterial(uint32_t index, std::shared_ptr<Material> material)
	{
		if (index >= m_materials.size())
		{
			m_materials.resize(static_cast<size_t>(index + 1));
		}

		m_materials[index] = material->CreateClone();
	}
	std::shared_ptr<Material> RendererComponent::GetMaterial(uint32_t index)
	{
		if (index >= m_materials.size())
		{
			return nullptr;
		}

		return m_materials[index];
	}

	void RendererComponent::OnImGui()
	{
		//=================================================
		// Materials
		//=================================================
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