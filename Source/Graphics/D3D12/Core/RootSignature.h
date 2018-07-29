#ifndef __D3D12_ROOT_SIGNATURE__
#define __D3D12_ROOT_SIGNATURE__

#include "Core/Memory/Memory.h"
#include <d3d12.h>

namespace Engine
{
	class RootParameter
	{
		friend class RootSignature;

	private:
		D3D12_ROOT_PARAMETER mParameter;

	public:
		RootParameter()
		{
			mParameter.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xffffffff;
		}

		~RootParameter()
		{
			Clear();
		}

		void Clear()
		{
			if (mParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				delete[] mParameter.DescriptorTable.pDescriptorRanges;
			}

			mParameter.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xffffffff;
		}

		void InitAsConstants(unsigned int reg, unsigned int numDwords, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			mParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			mParameter.ShaderVisibility = visibility;
			mParameter.Constants.Num32BitValues = numDwords;
			mParameter.Constants.ShaderRegister = reg;
			mParameter.Constants.RegisterSpace = 0;
		}

		void InitAsConstantBuffer(unsigned int reg, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			mParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			mParameter.ShaderVisibility = visibility;
			mParameter.Descriptor.ShaderRegister = reg;
			mParameter.Descriptor.RegisterSpace = 0;
		}

		void InitAsSRVBuffer(unsigned int reg, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			mParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
			mParameter.ShaderVisibility = visibility;
			mParameter.Descriptor.ShaderRegister = reg;
			mParameter.Descriptor.RegisterSpace = 0;
		}

		void InitAsUAVBuffer(unsigned int reg, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			mParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
			mParameter.ShaderVisibility = visibility;
			mParameter.Descriptor.ShaderRegister = reg;
			mParameter.Descriptor.RegisterSpace = 0;
		}

		void InitAsDescriptorTable(unsigned int rangeCount, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			mParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			mParameter.ShaderVisibility = visibility;
			mParameter.DescriptorTable.NumDescriptorRanges = rangeCount;
			mParameter.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[rangeCount];
		}

		void SetTableRange(unsigned int rangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE type, unsigned int reg, unsigned int count, unsigned int space = 0)
		{
			D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(mParameter.DescriptorTable.pDescriptorRanges + rangeIndex);
			range->RangeType = type;
			range->NumDescriptors = count;
			range->BaseShaderRegister = reg;
			range->RegisterSpace = space;
			range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		}

		const D3D12_ROOT_PARAMETER& operator() () const { return mParameter; }

		ALIGNED_NEW_DELETE("Engine::Graphics::RootParameter")
	};

	class RootSignature
	{
	private:
		ID3D12Device* mDevice;
		ID3D12RootSignature* mRootSignature;
		bool mFinalized;
		unsigned int mNumParameters;
		unsigned int mNumSamplers;
		unsigned int mNumInitializedStaticSamplers;
		unsigned int mDescriptorTableBitMap;
		unsigned int mSamplerTableBitMap;
		unsigned int mDescriptorTableSize[16];
		RootParameter* mParamArray;
		D3D12_STATIC_SAMPLER_DESC* mSamplerArray;

	public:
		RootSignature(ID3D12Device* device, unsigned int numRootParams = 0, unsigned int numStaticSamplers = 0)
		{
			mDevice = device;
			mNumParameters = numRootParams;
			mNumSamplers = numStaticSamplers;

			mFinalized = false;
			mParamArray = NULL;
			mSamplerArray = NULL;

			Reset(numRootParams, numStaticSamplers);
		}

		~RootSignature()
		{
			if (mParamArray)
			{
				delete[] mParamArray;
			}

			if (mSamplerArray)
			{
				delete[] mSamplerArray;
			}

			if (mFinalized)
			{
				mRootSignature->Release();
			}
		}

		void Reset(unsigned int numRootParams, unsigned int numStaticSamplers = 0)
		{
			if (mParamArray)
			{
				delete[] mParamArray;
			}

			if (numRootParams > 0)
			{
				mParamArray = new RootParameter[numRootParams];
			}
			else
			{
				mParamArray = NULL;
			}
			mNumParameters = numRootParams;

			if (mSamplerArray)
			{
				delete[] mSamplerArray;
			}

			if (numStaticSamplers > 0)
			{
				mSamplerArray = new D3D12_STATIC_SAMPLER_DESC[numStaticSamplers];
			}
			else
			{
				mSamplerArray = NULL;
			}
			mNumSamplers = numStaticSamplers;
			mNumInitializedStaticSamplers = 0;
		}

		RootParameter& operator[](size_t index)
		{
			return mParamArray[index];
		}

		const RootParameter& operator[](size_t index) const
		{
			return mParamArray[index];
		}

		void InitStaticSampler(unsigned int reg, const D3D12_SAMPLER_DESC& samplerDesc, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

		void Finalize(D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);

		ID3D12RootSignature* Get()
		{
			return mRootSignature;
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::RootSignature")
	};
}

#endif