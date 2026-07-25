#pragma once

#include <vector>

namespace Engine
{
	class Mesh;
	class RendererComponent;

	struct RenderItem
	{
		RendererComponent* renderer;

		size_t meshIndex = 0;
		size_t subMeshIndex = 0;
	};

	class RenderQueue
	{
	public:
		void Clear()
		{
			m_opaque.clear();
			m_transparent.clear();
		}

		void AddOpaque(const RenderItem& item)
		{
			m_opaque.push_back(item);
		}

		void AddTransparent(const RenderItem& item)
		{
			m_transparent.push_back(item);
		}

		const std::vector<RenderItem>& GetOpaqueQueue() const
		{
			return m_opaque;
		}

		const std::vector<RenderItem>& GetTransparentQueue() const
		{
			return m_transparent;
		}

	private:
		std::vector<RenderItem> m_opaque;
		std::vector<RenderItem> m_transparent;
	};
}