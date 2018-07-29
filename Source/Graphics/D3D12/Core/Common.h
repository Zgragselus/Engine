#ifndef __COMMON__H__
#define __COMMON__H__

#include <d3d12.h>

namespace Engine
{
	class Graphics
	{
	private:
		static DXGI_FORMAT FormatD3D[];
		static size_t FormatSize[];
		static D3D12_PRIMITIVE_TOPOLOGY TopologyD3D[];

	public:
		enum Format
		{
			R8 = 0,
			RG8,
			RGBA8,
			R16,
			RG16,
			RGBA16,
			R32,
			RG32,
			RGB32,
			RGBA32,
			R16F,
			RG16F,
			RGBA16F,
			R32F,
			RG32F,
			RGB32F,
			RGBA32F,
			D24S8,
			D32F
		};

		static inline size_t GetFormatSize(Format format)
		{
			return FormatSize[format];
		}

		static inline DXGI_FORMAT GetFormat(Format format)
		{
			return FormatD3D[format];
		}

		enum Topology
		{
			UNDEFINED,
			POINTLIST,
			LINELIST,
			LINESTRIP,
			TRIANGLELIST,
			TRIANGLESTRIP,
			LINELIST_ADJ,
			LINESTRIP_ADJ,
			TRIANGLELIST_ADJ,
			TRIANGLESTRIP_ADJ,
			PATCHLIST_1,
			PATCHLIST_2,
			PATCHLIST_3,
			PATCHLIST_4,
			PATCHLIST_5,
			PATCHLIST_6,
			PATCHLIST_7,
			PATCHLIST_8,
			PATCHLIST_9,
			PATCHLIST_10,
			PATCHLIST_11,
			PATCHLIST_12,
			PATCHLIST_13,
			PATCHLIST_14,
			PATCHLIST_15,
			PATCHLIST_16,
			PATCHLIST_17,
			PATCHLIST_18,
			PATCHLIST_19,
			PATCHLIST_20,
			PATCHLIST_21,
			PATCHLIST_22,
			PATCHLIST_23,
			PATCHLIST_24,
			PATCHLIST_25,
			PATCHLIST_26,
			PATCHLIST_27,
			PATCHLIST_28,
			PATCHLIST_29,
			PATCHLIST_30,
			PATCHLIST_31,
			PATCHLIST_32
		};

		static inline D3D12_PRIMITIVE_TOPOLOGY GetTopology(Topology format)
		{
			return TopologyD3D[format];
		}
	};
}

#endif