#include "ResourceDesc.h"

namespace Engine
{
	ResourceDesc ResourceDesc::CreateColorTarget(uint32_t width, uint32_t height, DXGI_FORMAT format)
	{
		ResourceDesc desc;
		desc.width = width;
		desc.height = height;
		desc.format = format;
		desc.flags = ResourceFlags::RTV | ResourceFlags::SRV;
		return desc;
	}

	ResourceDesc ResourceDesc::CreateDepthStencil(uint32_t width, uint32_t height, DXGI_FORMAT format)
	{
		ResourceDesc desc;
		desc.width = width;
		desc.height = height;
		desc.format = format;
		desc.flags = ResourceFlags::DSV | ResourceFlags::SRV;
		return desc;
	}

	ResourceDesc ResourceDesc::CreateBackBuffer(uint32_t width, uint32_t height, DXGI_FORMAT format)
	{
		ResourceDesc desc;
		desc.width = width;
		desc.height = height;
		desc.format = format;
		desc.flags = ResourceFlags::RTV;
		return desc;
	}
}