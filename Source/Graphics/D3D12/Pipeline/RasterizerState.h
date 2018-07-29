///////////////////////////////////////////////////////////////////////////////////////////////////
//
// RasterizerState.h
//
// Generic rasterizer state descriptor
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __RASTERIZER_STATE__H__
#define __RASTERIZER_STATE__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Memory/Memory.h"
#include <d3d12.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Rasterizer state class allows for generating D3D12 rasterizer states
	/// </summary>
	class RasterizerState
	{
	public:
		/// <summary>
		/// Rasterization fill mode - whether wireframe or fill (solid) mode
		/// </summary>
		enum FillMode
		{
			WIREFRAME = 0,
			SOLID = 1
		};

		static D3D12_FILL_MODE FillModeD3D[2];

		/// <summary>
		/// Rasterization cull mode - whether to cull back faces, front faces or no culling at all
		/// </summary>
		enum CullMode
		{
			NONE = 0,
			FRONT = 1,
			BACK = 2
		};

		static D3D12_CULL_MODE CullModeD3D[3];

		/// <summary>
		/// Whether to use conservative rasterization or not
		/// </summary>
		enum ConservativeRasterizationMode
		{
			OFF = 0,
			ON = 1
		};

		static D3D12_CONSERVATIVE_RASTERIZATION_MODE ConservativeRasterizationD3D[2];

		/// <summary>
		/// Create D3D state for rasterizer
		/// </summary>
		/// <param name="fillMode">Wireframe or solid rendering mode</param>
		/// <param name="cullMode">Rasterization culling mode - front, back or none</param>
		/// <param name="frontCounterClockwise">Whether front faces are counter-clock-wise ordered</param>
		/// <param name="depthBias">Depth value added to pixel</param>
		/// <param name="depthBiasClamp">Maximum depth value added to pixel</param>
		/// <param name="slopeScaledDepthBias">Scale on given pixel slope</param>
		/// <param name="depthClipEnable">Enable clipping on depth distance?</param>
		/// <param name="multisampleEnable">Enable multisampling</param>
		/// <param name="antialiasedLineEnable">Line based AA, only if multisampling is enabled</param>
		/// <param name="forcedSampleCount">Sample count that is forced while rendering or rasterizing, 0 - no value is forced</param>
		/// <param name="conservativeRaster">Whether conservative rasterization is OFF or ON</param>
		static D3D12_RASTERIZER_DESC CreateStateD3D(FillMode fillMode = SOLID,
			CullMode cullMode = BACK,
			bool frontCounterClockwise = true,
			int depthBias = 0,
			float depthBiasClamp = 0.0f,
			float slopeScaledDepthBias = 0.0f,
			bool depthClipEnable = true,
			bool multisampleEnable = false,
			bool antialiasedLineEnable = false,
			unsigned int forcedSampleCount = 0,
			ConservativeRasterizationMode conservativeRaster = OFF)
		{
			D3D12_RASTERIZER_DESC desc;
			desc.FillMode = FillModeD3D[fillMode];
			desc.CullMode = CullModeD3D[cullMode];
			desc.FrontCounterClockwise = frontCounterClockwise;
			desc.DepthBias = depthBias;
			desc.DepthBiasClamp = depthBiasClamp;
			desc.SlopeScaledDepthBias = slopeScaledDepthBias;
			desc.DepthClipEnable = depthClipEnable;
			desc.MultisampleEnable = multisampleEnable;
			desc.AntialiasedLineEnable = antialiasedLineEnable;
			desc.ForcedSampleCount = forcedSampleCount;
			desc.ConservativeRaster = ConservativeRasterizationD3D[conservativeRaster];
			return desc;
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::RasterizerState")
	};
}

#endif