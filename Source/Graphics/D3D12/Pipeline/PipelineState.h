///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PipelineState.h
//
// Generic pipeline state descriptor
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PIPELINE_STATE__H__
#define __PIPELINE_STATE__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Memory/Memory.h"
#include <d3d12.h>
#include <vector>
#include "../Core/RootSignature.h"
#include "../Core/D3DShader.h"
#include "../Core/Common.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	class DepthStencilState
	{
	public:
		enum Comparison
		{
			NEVER = 0,
			LESS,
			EQUAL,
			LEQUAL,
			GREATER,
			NOTEQUAL,
			GEQUAL,
			ALWAYS
		};

		static D3D12_COMPARISON_FUNC ComparisonD3D[];

		enum Mask
		{
			ZERO = 0,
			ALL
		};

		static D3D12_DEPTH_WRITE_MASK MaskD3D[];

		enum StencilOp
		{
			OP_KEEP = 0,
			OP_ZERO,
			OP_REPLACE,
			OP_INCR_CLAMP,
			OP_DECR_CLAMP,
			OP_INVERT,
			OP_INCR,
			OP_DECR
		};

		static D3D12_STENCIL_OP StencilOpD3D[];

		struct StencilDesc
		{
			StencilOp mFail;
			StencilOp mZFail;
			StencilOp mZPass;
			Comparison mFunc;

			StencilDesc(StencilOp fail = OP_KEEP,
				StencilOp zfail = OP_KEEP,
				StencilOp zpass = OP_KEEP,
				Comparison func = ALWAYS)
			{
				mFail = fail;
				mZFail = zfail;
				mZPass = zpass;
				mFunc = func;
			}
		};

		bool mDepthEnabled;
		Mask mDepthMask;
		Comparison mDepthFunc;

		bool mStencilEnabled;
		unsigned char mStencilReadMask;
		unsigned char mStencilWriteMask;
		StencilDesc mFrontFace;
		StencilDesc mBackFace;

		DepthStencilState(bool depthEnabled = true,
			Mask depthMask = ALL,
			Comparison depthFunc = LEQUAL,
			bool stencilEnabled = false,
			unsigned char stencilReadMask = 0x00,
			unsigned char stencilWriteMask = 0x00,
			StencilDesc frontFace = StencilDesc(),
			StencilDesc backFace = StencilDesc())
		{
			mDepthEnabled = depthEnabled;
			mDepthMask = depthMask;
			mDepthFunc = depthFunc;

			mStencilEnabled = stencilEnabled;
			mStencilReadMask = stencilReadMask;
			mStencilWriteMask = stencilWriteMask;
			mFrontFace = frontFace;
			mBackFace = backFace;
		}

		inline D3D12_DEPTH_STENCIL_DESC Get()
		{
			D3D12_DEPTH_STENCIL_DESC desc;
			desc.DepthEnable = mDepthEnabled;
			desc.DepthWriteMask = MaskD3D[mDepthMask];
			desc.DepthFunc = ComparisonD3D[mDepthFunc];
			desc.StencilEnable = mStencilEnabled;
			desc.StencilReadMask = mStencilReadMask;
			desc.StencilWriteMask = mStencilWriteMask;
			desc.FrontFace.StencilFunc = ComparisonD3D[mFrontFace.mFunc];
			desc.FrontFace.StencilPassOp = StencilOpD3D[mFrontFace.mZPass];
			desc.FrontFace.StencilFailOp = StencilOpD3D[mFrontFace.mFail];
			desc.FrontFace.StencilDepthFailOp = StencilOpD3D[mFrontFace.mZFail];
			desc.BackFace.StencilFunc = ComparisonD3D[mBackFace.mFunc];
			desc.BackFace.StencilPassOp = StencilOpD3D[mBackFace.mZPass];
			desc.BackFace.StencilFailOp = StencilOpD3D[mBackFace.mFail];
			desc.BackFace.StencilDepthFailOp = StencilOpD3D[mBackFace.mZFail];
			return desc;
		}
	};

	class InputLayout
	{
	public:
		enum Classification
		{
			PER_VERTEX,
			PER_INSTANCE
		};

		static D3D12_INPUT_CLASSIFICATION ClassificationD3D[];

		struct Attribute
		{
			std::string mSemanticName;
			unsigned int mSemanticIndex;
			Graphics::Format mFormat;
			unsigned int mSlot;
			unsigned int mStride;
			Classification mClassification;
			unsigned int mInstanceStepRate;

			Attribute(const char* semanticName, 
				unsigned int semanticIndex,
				Graphics::Format format,
				unsigned int slot,
				unsigned int stride,
				Classification classification,
				unsigned int instanceStepRate)
			{
				mSemanticName = std::string(semanticName);
				mSemanticIndex = semanticIndex;
				mFormat = format;
				mSlot = slot;
				mStride = stride;
				mClassification = classification;
				mInstanceStepRate = instanceStepRate;
			}
		};

		std::vector<Attribute> mAttributes;
		std::vector<D3D12_INPUT_ELEMENT_DESC> mElements;
		
		void AddAttribute(const Attribute& attr)
		{
			mAttributes.push_back(attr);
		}

		inline D3D12_INPUT_LAYOUT_DESC Get()
		{
			mElements.resize(mAttributes.size());
			for (int i = 0; i < mAttributes.size(); i++)
			{
				mElements[i].SemanticName = mAttributes[i].mSemanticName.c_str();
				mElements[i].SemanticIndex = mAttributes[i].mSemanticIndex;
				mElements[i].Format = Graphics::GetFormat(mAttributes[i].mFormat);
				mElements[i].InputSlot = mAttributes[i].mSlot;
				mElements[i].AlignedByteOffset = mAttributes[i].mStride;
				mElements[i].InputSlotClass = ClassificationD3D[mAttributes[i].mClassification];
				mElements[i].InstanceDataStepRate = mAttributes[i].mInstanceStepRate;
			}

			D3D12_INPUT_LAYOUT_DESC desc;

			desc.NumElements = (unsigned int)mAttributes.size();
			desc.pInputElementDescs = &mElements[0];

			return desc;
		}
	};

	/// <summary>
	/// Pipeline state factory
	/// </summary>
	class PipelineState
	{
	public:
		static D3D12_PRIMITIVE_TOPOLOGY_TYPE D3DPrimitiveType[4];

		enum PrimitiveType
		{
			POINT = 0,
			LINE,
			TRIANGLE,
			PATCH
		};

		ID3D12PipelineState* mState;

		PipelineState(ID3D12Device* device,
			RootSignature* rootSignature,
			D3DShader* shader,
			D3D12_BLEND_DESC blendDesc,
			D3D12_RASTERIZER_DESC rasterDesc,
			DepthStencilState* dsState,
			InputLayout* inputLayout,
			PrimitiveType primType,
			std::vector<Graphics::Format>& rtFormat,
			Graphics::Format dsvFormat,
			unsigned int msaaSamples,
			unsigned int msaaQuality)
		{
			DXGI_SAMPLE_DESC samplerDesc;
			samplerDesc.Count = msaaSamples;
			samplerDesc.Quality = msaaQuality;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = { 0 };
			desc.pRootSignature = rootSignature->Get();
			desc.VS.BytecodeLength = shader->GetBytecodeLength(D3DShader::VERTEX_SHADER);
			desc.VS.pShaderBytecode = shader->GetBytecodePtr(D3DShader::VERTEX_SHADER);
			desc.HS.BytecodeLength = shader->GetBytecodeLength(D3DShader::HULL_SHADER);
			desc.HS.pShaderBytecode = shader->GetBytecodePtr(D3DShader::HULL_SHADER);
			desc.DS.BytecodeLength = shader->GetBytecodeLength(D3DShader::DOMAIN_SHADER);
			desc.DS.pShaderBytecode = shader->GetBytecodePtr(D3DShader::DOMAIN_SHADER);
			desc.GS.BytecodeLength = shader->GetBytecodeLength(D3DShader::GEOMETRY_SHADER);
			desc.GS.pShaderBytecode = shader->GetBytecodePtr(D3DShader::GEOMETRY_SHADER);
			desc.PS.BytecodeLength = shader->GetBytecodeLength(D3DShader::PIXEL_SHADER);
			desc.PS.pShaderBytecode = shader->GetBytecodePtr(D3DShader::PIXEL_SHADER);
			desc.BlendState = blendDesc;
			desc.SampleMask = UINT_MAX;
			desc.RasterizerState = rasterDesc;
			desc.DepthStencilState = dsState->Get();
			desc.InputLayout = inputLayout->Get();
			desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
			desc.PrimitiveTopologyType = D3DPrimitiveType[primType];
			desc.NumRenderTargets = (unsigned int)rtFormat.size();
			for (int i = 0; i < rtFormat.size(); i++)
			{
				desc.RTVFormats[i] = Graphics::GetFormat(rtFormat[i]);
			}
			desc.DSVFormat = Graphics::GetFormat(dsvFormat);
			desc.SampleDesc = samplerDesc;
			desc.NodeMask = 0;
			desc.CachedPSO = { 0 };
			desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

			device->CreateGraphicsPipelineState(&desc, __uuidof(ID3D12PipelineState), (void**)&this->mState);
		}

		PipelineState(ID3D12Device* device,
			RootSignature* rootSignature,
			D3DShader* shader)
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = { 0 };
			desc.pRootSignature = rootSignature->Get();
			desc.CS.BytecodeLength = shader->GetBytecodeLength(D3DShader::COMPUTE_SHADER);
			desc.CS.pShaderBytecode = shader->GetBytecodePtr(D3DShader::COMPUTE_SHADER);
			desc.NodeMask = 0;
			desc.CachedPSO = { 0 };
			desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

			device->CreateComputePipelineState(&desc, __uuidof(ID3D12PipelineState), (void**)&this->mState);
		}

		~PipelineState()
		{
			mState->Release();
		}
		
		inline ID3D12PipelineState* Get()
		{
			return mState;
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::PipelineState")
	};
}

#endif