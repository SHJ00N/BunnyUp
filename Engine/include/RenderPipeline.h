#pragma once

#include <d3d11.h>
#include <memory>
#include "ConstantBufferManager.h"

namespace Engine
{
	class Scene;
	class PassRenderer;
	class ConstantBufferManager;
	class TextRenderer;
	class RenderPipeline
	{
	public:
		RenderPipeline();
		~RenderPipeline();

		void Initialize(ConstantBufferManager* cbManager);
		void Render(Scene* scene);
	private:
		void build(PassRenderer& renderer, Scene* scene);

		std::unique_ptr<TextRenderer> m_textRenderer;
		ConstantBufferManager *m_cbManager = nullptr;
	};
}