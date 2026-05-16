#pragma once

#include <d3d11.h>
#include <memory>
#include "ConstantBufferManager.h"

namespace Engine
{
	class Scene;
	class RenderGraph;
	class ConstantBufferManager;
	class RenderPipeline
	{
	public:
		HRESULT Initialize();
		void Render(Scene* scene);
	private:
		void build(RenderGraph& graph, Scene* scene);

		std::unique_ptr<ConstantBufferManager> m_cbManager;
	};
}