#include "pch.h"
#include "Socket.h"
#include "Animator.h"
#include "GameObject.h"
#include "ImGuiClass.h"
#include "Log.h"

namespace Engine
{
	void SocketComponent::Attach(const std::string& name, int boneIndex, GameObject* object)
	{
		auto& socket = m_sockets[name];
		socket.boneIndex = boneIndex;
		socket.attachedObject = object;
	}

	void SocketComponent::Attach(const std::string& name, int boneIndex, GameObject* object, const Vector3& pos, const Vector3& rot, const Vector3& scale)
	{
		auto& socket = m_sockets[name];
		socket.boneIndex = boneIndex;
		socket.attachedObject = object;
		socket.position = pos;
		socket.rotation = rot;
		socket.scale = scale;
	}
	
	void SocketComponent::Detach(const std::string& name)
	{
		auto it = m_sockets.find(name);
		if (it != m_sockets.end())
		{
			it->second.attachedObject = nullptr;
		}
	}
	
	void SocketComponent::OnStart()
	{
		m_animator = ownerGameObject->GetComponent<Animator>();
	}

	void SocketComponent::Update(float dt)
	{
		if (!m_animator) return;

		const auto& boneMatrices = m_animator->GetGlobalBoneMatrices();
		const auto& world = ownerGameObject->transform.GetWorldMatrix();

		for (auto& [name, socket] : m_sockets)
		{
			if (!socket.attachedObject) continue;
			if (socket.boneIndex < 0 || socket.boneIndex >= static_cast<int>(boneMatrices.size())) continue;

			auto worldMatrix = TransformMatrix(socket.position, socket.rotation, socket.scale) * boneMatrices[socket.boneIndex] * world;
			Vector3 position, scale; 
			Quaternion rotation;
			Decompose(worldMatrix, position, rotation, scale);

			auto& transform = socket.attachedObject->transform;
			transform.SetLocalPosition(position);
			transform.SetLocalRotation(rotation);
			transform.SetLocalScale(scale);
		}
	}

	void SocketComponent::OnImGui()
	{
		for (auto& [name, socket] : m_sockets)
		{
			if (ImGui::TreeNode(name.c_str()))
			{
				ImGui::DragFloat3("Position", &socket.position.x, 0.01f);
				ImGui::DragFloat3("Rotation", &socket.rotation.x, 0.01f);
				ImGui::DragFloat3("Scale", &socket.scale.x, 0.01f, 1.0f, 1000.0f);

				ImGui::InputInt("Bone Index", &socket.boneIndex);

				if (socket.attachedObject)
					ImGui::Text("Attached : %s",
						socket.attachedObject->GetName().c_str());
				else
					ImGui::Text("Attached : None");

				ImGui::TreePop();
			}
		}
	}
}