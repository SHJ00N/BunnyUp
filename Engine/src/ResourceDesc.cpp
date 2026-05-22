#include "ResourceDesc.h"

namespace Engine
{
	ResourceDesc ResourceDesc::CreateColorTarget(uint32_t width, uint32_t height, DXGI_FORMAT format, ResourceType type, unsigned int mipLevels)
	{
		ResourceDesc desc;
		desc.width = width;
		desc.height = height;
		desc.format = format;
		desc.type = type;
		desc.mipLevels = mipLevels;
		desc.flags = ResourceFlags::RTV | ResourceFlags::SRV;
		return desc;
	}

	ResourceDesc ResourceDesc::CreateDepthStencil(uint32_t width, uint32_t height, DXGI_FORMAT format, ResourceType type, unsigned int mipLevels)
	{
		ResourceDesc desc;
		desc.width = width;
		desc.height = height;
		desc.format = format;
		desc.type = type;
		desc.mipLevels = mipLevels;
		desc.flags = ResourceFlags::DSV | ResourceFlags::SRV;
		return desc;
	}

	ResourceDesc ResourceDesc::CreateWriteOnlyBuffer(uint32_t width, uint32_t height, DXGI_FORMAT format, ResourceType type, unsigned int mipLevels)
	{
		ResourceDesc desc;
		desc.width = width;
		desc.height = height;
		desc.format = format;
		desc.type = type;
		desc.mipLevels = mipLevels;
		desc.flags = ResourceFlags::RTV;
		return desc;
	}
}