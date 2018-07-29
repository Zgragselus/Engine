///////////////////////////////////////////////////////////////////////////////////////////////////
//
// BlendState.h
//
// Generic blend state descriptor
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __BLEND_STATE__H__
#define __BLEND_STATE__H__

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
	class BlendState
	{
	public:
		enum Blend
		{
			ZERO = 0,
			ONE,
			SRC_COLOR,
			INV_SRC_COLOR,
			SRC_ALPHA,
			INV_SRC_ALPHA,
			DEST_ALPHA,
			INV_DEST_ALPHA,
			DEST_COLOR,
			INV_DEST_COLOR,
			SRC_ALPHA_SAT,
			BLEND_FACTOR,
			INV_BLEND_FACTOR,
			SRC1_COLOR,
			INV_SRC1_COLOR,
			SRC1_ALPHA,
			INV_SRC1_ALPHA
		};

		static D3D12_BLEND BlendModeD3D[];

		enum BlendOp
		{
			OP_ADD = 0,
			OP_SUBTRACT,
			OP_REV_SUBTRACT,
			OP_MIN,
			OP_MAX
		};

		static D3D12_BLEND_OP BlendOpD3D[];

		enum LogicOp
		{
			OP_CLEAR = 0,
			OP_SET,
			OP_COPY,
			OP_COPY_INVERTED,
			OP_NOOP,
			OP_INVERT,
			OP_AND,
			OP_NAND,
			OP_OR,
			OR_NOR,
			OP_XOR,
			OP_EQUIV,
			OP_AND_REVERSE,
			OP_AND_INVERTED,
			OP_OR_REVERSE,
			OP_OR_INVERTED
		};

		static D3D12_LOGIC_OP LogicOpD3D[];

		enum ColorMask
		{
			RED = 1,
			GREEN = 2,
			BLUE = 4,
			ALPHA = 8,
			ALL = (((RED | GREEN) | BLUE) | ALPHA)
		};

		class BlendDesc
		{
		public:
			bool BlendEnable;
			bool LogicOpEnable;
			Blend ColorSrcBlend;
			Blend ColorDestBlend;
			BlendOp ColorBlendOp;
			Blend AlphaSrcBlend;
			Blend AlphaDestBlend;
			BlendOp AlphaBlendOp;
			LogicOp BlendLogicOp;
			unsigned char RTWriteMask;
			
			BlendDesc(bool blendEnable = false,
				bool logicOpEnable = false,
				Blend cSrc = ONE,
				Blend cDest = ZERO,
				BlendOp cOp = OP_ADD,
				Blend aSrc = ONE,
				Blend aDest = ZERO,
				BlendOp aOp = OP_ADD,
				LogicOp lOp = OP_NOOP,
				unsigned char rtWriteMask = ALL)
			{
				BlendEnable = blendEnable;
				LogicOpEnable = logicOpEnable;
				ColorSrcBlend = cSrc;
				ColorDestBlend = cDest;
				ColorBlendOp = cOp;
				AlphaSrcBlend = aSrc;
				AlphaDestBlend = aDest;
				AlphaBlendOp = aOp;
				BlendLogicOp = lOp;
				RTWriteMask = rtWriteMask;
			}

			D3D12_RENDER_TARGET_BLEND_DESC CreateStateD3D() const
			{
				D3D12_RENDER_TARGET_BLEND_DESC result;
				result.BlendEnable = this->BlendEnable;
				result.LogicOpEnable = this->LogicOpEnable;
				result.SrcBlend = BlendState::BlendModeD3D[this->ColorSrcBlend];
				result.DestBlend = BlendState::BlendModeD3D[this->ColorDestBlend];
				result.BlendOp = BlendState::BlendOpD3D[this->ColorBlendOp];
				result.SrcBlendAlpha = BlendState::BlendModeD3D[this->AlphaSrcBlend];
				result.DestBlendAlpha = BlendState::BlendModeD3D[this->AlphaDestBlend];
				result.BlendOpAlpha = BlendState::BlendOpD3D[this->AlphaBlendOp];
				result.LogicOp = BlendState::LogicOpD3D[this->BlendLogicOp];
				result.RenderTargetWriteMask = this->RTWriteMask;
				return result;
			}
		};

		static D3D12_BLEND_DESC CreateStateD3D(bool alphaToCoverageEnable = false,
			bool independentBlendEnable = false,
			const BlendDesc& blendDescRT0 = BlendDesc(),
			const BlendDesc& blendDescRT1 = BlendDesc(),
			const BlendDesc& blendDescRT2 = BlendDesc(),
			const BlendDesc& blendDescRT3 = BlendDesc(),
			const BlendDesc& blendDescRT4 = BlendDesc(),
			const BlendDesc& blendDescRT5 = BlendDesc(),
			const BlendDesc& blendDescRT6 = BlendDesc(),
			const BlendDesc& blendDescRT7 = BlendDesc())
		{
			D3D12_BLEND_DESC result;
			result.AlphaToCoverageEnable = alphaToCoverageEnable;
			result.IndependentBlendEnable = independentBlendEnable;
			result.RenderTarget[0] = blendDescRT0.CreateStateD3D();
			result.RenderTarget[1] = blendDescRT1.CreateStateD3D();
			result.RenderTarget[2] = blendDescRT2.CreateStateD3D();
			result.RenderTarget[3] = blendDescRT3.CreateStateD3D();
			result.RenderTarget[4] = blendDescRT4.CreateStateD3D();
			result.RenderTarget[5] = blendDescRT5.CreateStateD3D();
			result.RenderTarget[6] = blendDescRT6.CreateStateD3D();
			result.RenderTarget[7] = blendDescRT7.CreateStateD3D();
			return result;
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::BlendState")
	};
}

#endif