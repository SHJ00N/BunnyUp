#pragma once

#include "Component.h"

#include <string>
#include <unordered_map>

namespace Engine
{
	class Animator;
	
	struct Socket
	{
		int boneIndex = -1;
		GameObject* attachedObject = nullptr;

		// offset
		Vector3 position;
		Vector3 rotation;
		Vector3 scale = Vector3(1.0f);
	};

	class SocketComponent : public Component
	{
	public:
		void Update(float dt) override;
		void OnImGui() override;

		void Attach(const std::string& name, int boneIndex, GameObject* object);
		void Attach(const std::string& name, int boneIndex, GameObject* object, const Vector3& pos, const Vector3& rot, const Vector3& scale);
		void Detach(const std::string& name);

	protected:
		void OnStart() override;

	private:
		Animator* m_animator = nullptr;
		std::unordered_map<std::string, Socket> m_sockets;
	};
}