#pragma once

#include <vector>
#include <unordered_set>
#include <string>
#include <functional>
#include <queue>
#include <algorithm>
#include <assert.h>
#include <memory>

#include "RenderCommandList.h"
#include "ResourceDesc.h"

namespace Engine
{
	using ResourceHandle = uint32_t;
	
	class Shader;
	class RenderTargetResource;

	enum class PassType
	{
		Graphics,
		Compute,
		Copy
	};

	struct ResourceEntry
	{
		std::string name;
		ResourceDesc desc;

		bool isImported = false;
		std::unique_ptr<RenderTargetResource> resource = nullptr;
	};

	struct RenderPassParameter
	{
		std::vector<ResourceHandle> reads;
		std::vector<ResourceHandle> writes;
	};

	struct RenderPass
	{
		std::string name;

		PassType type;

		std::vector<ResourceHandle> reads;
		std::vector<ResourceHandle> writes;

		Shader* shader = nullptr;

		std::function<void(RenderCommandList&)> execute;
	};

	class PassRenderer
	{
	public:
		ResourceHandle Create(const std::string& name, const ResourceDesc& desc);
		ResourceHandle Import(const std::string& name, const ResourceDesc& desc, ID3D11Texture2D* texture, ID3D11RenderTargetView* rtv = nullptr, ID3D11DepthStencilView* dsv = nullptr, ID3D11ShaderResourceView* srv = nullptr);
		void AddPass(const std::string& name, PassType type, const RenderPassParameter& parameter, std::function<void(RenderCommandList&)> execute, Shader* shader = nullptr);

		void Compile(ID3D11Device* device);
		void Execute(ID3D11DeviceContext* context);
		void SetFinalOutput(ResourceHandle handle);
		void MarkActivePasses(const std::vector<int>& lastWriter);


		RenderTargetResource* GetResource(ResourceHandle handle);
		const ResourceDesc& GetResourceDesc(ResourceHandle handle) const;

	private:
		std::vector<RenderPass> m_passes;
		std::vector<ResourceEntry> m_resources;
		std::vector<ResourceHandle> m_finalOutputs;
		std::vector<bool> m_passActiveState;
		std::vector<bool> m_resourceActiveState;
		std::vector<int> m_executionOrder;
	};
}