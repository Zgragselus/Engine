#ifndef __SAMPLER__H__
#define __SAMPLER__H__

#include "Core/Memory/Memory.h"
#include <d3d12.h>

namespace Engine
{
	class Sampler
	{
	public:
		enum Filter
		{
			MIN_POINT_MAG_POINT_MIP_POINT,
			MIN_POINT_MAG_POINT_MIP_LINEAR,
			MIN_POINT_MAG_LINEAR_MIP_POINT,
			MIN_POINT_MAG_LINEAR_MIP_LINEAR,
			MIN_LINEAR_MAG_POINT_MIP_POINT,
			MIN_LINEAR_MAG_POINT_MIP_LINEAR,
			MIN_LINEAR_MAG_LINEAR_MIP_POINT,
			MIN_LINEAR_MAG_LINEAR_MIP_LINEAR,
			ANISOTROPIC,
			COMPARE_MIN_POINT_MAG_POINT_MIP_POINT,
			COMPARE_MIN_POINT_MAG_POINT_MIP_LINEAR,
			COMPARE_MIN_POINT_MAG_LINEAR_MIP_POINT,
			COMPARE_MIN_POINT_MAG_LINEAR_MIP_LINEAR,
			COMPARE_MIN_LINEAR_MAG_POINT_MIP_POINT,
			COMPARE_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
			COMPARE_MIN_LINEAR_MAG_LINEAR_MIP_POINT,
			COMPARE_MIN_LINEAR_MAG_LINEAR_MIP_LINEAR,
			COMPARE_ANISOTROPIC,
			MINIMUM_MIN_POINT_MAG_POINT_MIP_POINT,
			MINIMUM_MIN_POINT_MAG_POINT_MIP_LINEAR,
			MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
			MINIMUM_MIN_POINT_MAG_LINEAR_MIP_LINEAR,
			MINIMUM_MIN_LINEAR_MAG_POINT_MIP_POINT,
			MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
			MINIMUM_MIN_LINEAR_MAG_LINEAR_MIP_POINT,
			MINIMUM_MIN_LINEAR_MAG_LINEAR_MIP_LINEAR,
			MINIMUM_ANISOTROPIC,
			MAXIMUM_MIN_POINT_MAG_POINT_MIP_POINT,
			MAXIMUM_MIN_POINT_MAG_POINT_MIP_LINEAR,
			MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
			MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_LINEAR,
			MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_POINT,
			MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
			MAXIMUM_MIN_LINEAR_MAG_LINEAR_MIP_POINT,
			MAXIMUM_MIN_LINEAR_MAG_LINEAR_MIP_LINEAR,
			MAXIMUM_ANISOTROPIC
		};

		static D3D12_FILTER FilterD3D[36];

		enum Address
		{
			WRAP,
			MIRROR,
			CLAMP,
			BORDER,
			MIRROR_ONCE
		};

		static D3D12_TEXTURE_ADDRESS_MODE AddressD3D[5];

		enum ComparisonFunc
		{
			NEVER,
			LESS,
			EQUAL,
			LESS_EQUAL,
			GREATER,
			NOT_EQUAL,
			GREATER_EQUAL,
			ALWAYS
		};

		static D3D12_COMPARISON_FUNC ComparisonFuncD3D[8];
		
		static D3D12_SAMPLER_DESC CreateSamplerD3D(
			const Filter& filter,
			const Address& addressU,
			const Address& addressV,
			const Address& addressW,
			float LODbias,
			unsigned int maxAnisotropy,
			const ComparisonFunc& comparisonFunc,
			float borderColorR,
			float borderColorG,
			float borderColorB,
			float borderColorA,
			float minLOD,
			float maxLOD)
		{
			D3D12_SAMPLER_DESC desc;
			desc.Filter = FilterD3D[filter];
			desc.AddressU = AddressD3D[addressU];
			desc.AddressV = AddressD3D[addressV];
			desc.AddressW = AddressD3D[addressW];
			desc.MipLODBias = LODbias;
			desc.MaxAnisotropy = maxAnisotropy;
			desc.ComparisonFunc = ComparisonFuncD3D[comparisonFunc];
			desc.BorderColor[0] = borderColorR;
			desc.BorderColor[1] = borderColorG;
			desc.BorderColor[2] = borderColorB;
			desc.BorderColor[3] = borderColorA;
			desc.MinLOD = minLOD;
			desc.MaxLOD = maxLOD;
			return desc;
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::Sampler")
	};
}

#endif