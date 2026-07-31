#pragma once

#include <memory>
#include <vector>

namespace Engine
{
	class Shader;
	class Mesh;
	class TextComponent;
	class ConstantBufferManager;
		
	class TextRenderer
	{
	public:
		void Initialize();
		void Render(const std::vector<TextComponent*>& texts, ConstantBufferManager* cbManager);

	private:
		std::shared_ptr<Shader> m_shader;
		std::shared_ptr<Mesh> m_quadMesh;
	};
}