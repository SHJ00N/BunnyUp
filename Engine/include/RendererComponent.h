#pragma once

#include "Component.h"
#include "Material.h"

#include <d3d11.h>
#include <wrl/client.h>

namespace Engine
{
	class RendererComponent : public Component
	{
	public:
		void AddMaterial(std::shared_ptr<Material> material) { materials.push_back(material); }
		virtual void Render(class Renderer& renderer) { }

	protected:
		std::vector<std::weak_ptr<Material>>  materials;
	};
}