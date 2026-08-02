#include "pch.h"
#include "PassRenderer.h"
#include "RenderTargetResource.h"

namespace Engine
{
	ResourceHandle PassRenderer::Create(const std::string& name, const ResourceDesc& desc)
	{
		// Assign a new ID based on number of resources
		ResourceHandle id = static_cast<ResourceHandle>(m_resources.size());
		m_resources.push_back({ name, desc });
		return id;
	}
	ResourceHandle PassRenderer::Import(const std::string& name, const ResourceDesc& desc, ID3D11Texture2D* texture, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, ID3D11ShaderResourceView* srv)
	{
		// Assign a new ID based on number of resources
		ResourceHandle id = static_cast<ResourceHandle>(m_resources.size());
		// Create Resource Entry with imported
		ResourceEntry entry;
		entry.name = name;
		entry.desc = desc;
		entry.isImported = true;
		// Set entry's resource
		entry.resource = std::make_unique<RenderTargetResource>();
		entry.resource->WrapExternalResource(texture, rtv, dsv, srv);

		m_resources.push_back(std::move(entry));
		return id;
	}
	void PassRenderer::AddPass(const std::string& name, PassType type, const RenderPassParameter& parameter, std::function<void(RenderCommandList&)> execute, Shader* shader)
	{
		// Create new pass
		RenderPass pass;
		pass.name = name;
		pass.type = type;
		pass.reads = parameter.reads;
		pass.writes = parameter.writes;
		pass.execute = execute;
		pass.shader = shader;
		m_passes.push_back(pass);
	}
	void PassRenderer::Compile(ID3D11Device* device)
	{
		// Clear final execution order
		m_executionOrder.clear();
		// Last written pass of resource
		m_passActiveState.assign(m_passes.size(), false);
		m_resourceActiveState.assign(m_resources.size(), false);
		std::vector<int> lastWriter(m_resources.size(), -1);
		for (int i = 0; i < m_passes.size(); ++i)
		{
			for (auto res : m_passes[i].writes)
				lastWriter[res] = i;
		}
		// Check active pass
		MarkActivePasses(lastWriter);

		// Configure graph based on passActiveState container
		std::vector<std::unordered_set<int>> graph(m_passes.size());		// Adjacency nodes of each pass
		std::vector<int> indegree(m_passes.size(), 0);		// Degree of each pass

		for (int i = 0; i < m_passes.size(); ++i)
		{
			if (!m_passActiveState[i]) continue;

			auto& pass = m_passes[i];

			for (auto res : pass.reads)
			{
				int writer = lastWriter[res];
				if (writer != -1 && m_passActiveState[writer])
				{
					if (graph[writer].insert(i).second)
						indegree[i]++;
				}
			}
		}


		// Topological sort
		std::queue<int> passQueue;
		// Push 0 degree passes
		for (int i = 0; i < indegree.size(); ++i)
		{
			if (indegree[i] == 0 && m_passActiveState[i])
			{
				passQueue.push(i);
			}
		}
		// configure execution container
		while (!passQueue.empty())
		{
			int cur = passQueue.front();
			passQueue.pop();

			m_executionOrder.push_back(cur);

			for (int next : graph[cur])
			{
				if (--indegree[next] == 0 && m_passActiveState[next])
				{
					passQueue.push(next);
				}
			}
		}

		// Check cycle
		size_t activeCount = count(m_passActiveState.begin(), m_passActiveState.end(), true);
		assert(m_executionOrder.size() == activeCount && "RenderGraph cycle detected!");

		// Resources create
		for (size_t i = 0; i < m_resources.size(); ++i)
		{
			if (!m_resourceActiveState[i])
			{
				continue;
			}

			// Check imported resources
			auto& entry = m_resources[i];
			if (!entry.isImported && !entry.resource)
			{
				// Create resource
				entry.resource = std::make_unique<RenderTargetResource>();
				HRESULT hr = entry.resource->CreateResource(entry.desc, device);
				assert(SUCCEEDED(hr) && "Failed to create render graph resource");
			}
		}
	}

	void PassRenderer::Execute(ID3D11DeviceContext* context)
	{
		RenderCommandList command(context);
		for (int idx : m_executionOrder)
		{
			m_passes[idx].execute(command);
		}
	}

	void PassRenderer::SetFinalOutput(ResourceHandle handle)
	{
		m_finalOutputs.push_back(handle);
	}

	void PassRenderer::MarkActivePasses(const std::vector<int>& lastWriter)
	{
		std::vector<ResourceHandle> stack;

		assert(!m_finalOutputs.empty() && "Final output is empty");
		// Push final output resources
		for (auto res : m_finalOutputs)
		{
			stack.push_back(res);
		}

		while (!stack.empty())
		{
			auto res = stack.back();
			stack.pop_back();

			// Check resource state
			if (m_resourceActiveState[res]) continue;
			m_resourceActiveState[res] = true;

			// Check resource's last writer pass
			int writer = lastWriter[res];
			if (writer == -1) continue;

			if (m_passActiveState[writer]) continue;
			m_passActiveState[writer] = true;

			// tracking the reads of the writer pass
			for (auto input : m_passes[writer].reads)
			{
				stack.push_back(input);
			}
		}
	}

	RenderTargetResource* PassRenderer::GetResource(ResourceHandle handle)
	{
		assert(handle < m_resources.size() && "ResourceHandle is out of range");
		assert(m_resources[handle].resource && "Resource is null");
		return m_resources[handle].resource.get();
	}

	const ResourceDesc& PassRenderer::GetResourceDesc(ResourceHandle handle) const
	{
		return m_resources[handle].desc;
	}
}