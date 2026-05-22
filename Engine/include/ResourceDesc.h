#pragma once

#include <d3d11.h>
#include <stdint.h>

namespace Engine
{
	enum class ResourceFlags : uint32_t
	{
		None = 0,
		RTV  = 1 << 0,
		DSV  = 1 << 1,
		SRV  = 1 << 2
	};

	// utilities
	constexpr inline ResourceFlags operator|(ResourceFlags a, ResourceFlags b) noexcept
	{
		return static_cast<ResourceFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	constexpr inline ResourceFlags operator&(ResourceFlags a, ResourceFlags b) noexcept
	{
		return static_cast<ResourceFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
	}

	constexpr inline bool HasFlag(ResourceFlags flags, ResourceFlags flag)	noexcept
	{
		return (static_cast<uint32_t>(flags & flag) != 0);
	}

	enum class ResourceType
	{
		Texture2D,
		TextureCube,
	};

	struct ResourceDesc
	{
		uint32_t width = 0;
		uint32_t height = 0;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

		ResourceFlags flags = ResourceFlags::None;
		ResourceType type = ResourceType::Texture2D;
		unsigned int mipLevels = 1;

		static ResourceDesc CreateColorTarget(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, ResourceType type = ResourceType::Texture2D, unsigned int mipLevels = 1);
		static ResourceDesc CreateDepthStencil(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS, ResourceType type = ResourceType::Texture2D, unsigned int mipLevels = 1);
		static ResourceDesc CreateWriteOnlyBuffer(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM, ResourceType type = ResourceType::Texture2D, unsigned int mipLevels = 1);
	};
}