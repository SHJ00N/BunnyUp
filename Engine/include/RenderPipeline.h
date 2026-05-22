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
		void Initialize(ConstantBufferManager* cbManager);
		void Render(Scene* scene);
	private:
		void build(RenderGraph& graph, Scene* scene);

		ConstantBufferManager *m_cbManager = nullptr;
	};
}