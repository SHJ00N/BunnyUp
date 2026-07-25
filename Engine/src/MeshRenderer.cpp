#include "pch.h"
#include "MeshRenderer.h"
#include "ConstantBufferManager.h"
#include "ImGuiClass.h"
#include "ResourceManager.h"
#include "RenderQueue.h"

#include <algorithm>

namespace Engine
{
	MeshRenderer::MeshRenderer()
	{
	}

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
		m_model = model;

		m_meshes.clear();
		m_meshes = model->GetMeshes();
		
		m_materials.clear();
		for (auto& mat : model->GetMaterials())
		{
			m_materials.push_back(mat->CreateClone());
		}

		generateBound();
	}

	void MeshRenderer::Submit(RenderQueue& queue)
	{
		for (size_t meshIndex = 0; meshIndex < m_meshes.size(); ++meshIndex)
		{
			const auto& subMeshes = m_meshes[meshIndex]->subMeshes;
			for (size_t subMeshIndex = 0; subMeshIndex < subMeshes.size(); ++subMeshIndex)
			{
				const auto& sub = subMeshes[subMeshIndex];
				
				RenderItem item = { this, meshIndex, subMeshIndex };
				if (m_materials[sub.materialIndex]->IsTransparent())
				{
					queue.AddTransparent(item);
				}
				else
				{
					queue.AddOpaque(item);
				}
			}
		}
	}

	void MeshRenderer::Draw(const RenderItem& item, ConstantBufferManager& cbManager)
	{
		UpdateConstantBuffer(cbManager);

		m_meshes[item.meshIndex]->RenderSubMesh(item.subMeshIndex, m_materials, cbManager);
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
		//=================================================
		// Model
		//=================================================
		auto model = GetModel();
		std::string currentModel = model ? model->GetName() : "None";

		ImGui::Text("Model");
		ImGui::SameLine();

		ImGui::SetNextItemWidth(250.0f);

		if (ImGui::BeginCombo("##Model", currentModel.c_str()))
		{
			const auto& models = ResourceManager::GetInstance().GetModels();

			for (const auto& pair : models)
			{
				bool selected = (pair.second.get() == model);

				if (ImGui::Selectable(pair.first.c_str(), selected))
				{
					SetMesh(pair.second);
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::Separator();

		RendererComponent::OnImGui();
	}
}