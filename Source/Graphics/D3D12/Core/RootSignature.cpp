#include "RootSignature.h"
#include <cassert>

using namespace Engine;

void RootSignature::InitStaticSampler(unsigned int reg, const D3D12_SAMPLER_DESC& samplerDesc, D3D12_SHADER_VISIBILITY visibility)
{
	assert(mNumInitializedStaticSamplers < mNumSamplers);
	D3D12_STATIC_SAMPLER_DESC& staticSamplerDesc = mSamplerArray[mNumInitializedStaticSamplers++];

	staticSamplerDesc.Filter = samplerDesc.Filter;
	staticSamplerDesc.AddressU = samplerDesc.AddressU;
	staticSamplerDesc.AddressV = samplerDesc.AddressV;
	staticSamplerDesc.AddressW = samplerDesc.AddressW;
	staticSamplerDesc.MipLODBias = samplerDesc.MipLODBias;
	staticSamplerDesc.MaxAnisotropy = samplerDesc.MaxAnisotropy;
	staticSamplerDesc.ComparisonFunc = samplerDesc.ComparisonFunc;
	staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	staticSamplerDesc.MinLOD = samplerDesc.MinLOD;
	staticSamplerDesc.MaxLOD = samplerDesc.MaxLOD;
	staticSamplerDesc.ShaderRegister = reg;
	staticSamplerDesc.RegisterSpace = 0;
	staticSamplerDesc.ShaderVisibility = visibility;

	if (staticSamplerDesc.AddressU == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
		staticSamplerDesc.AddressV == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
		staticSamplerDesc.AddressW == D3D12_TEXTURE_ADDRESS_MODE_BORDER)
	{
		if (samplerDesc.BorderColor[3] == 1.0f)
		{
			if (samplerDesc.BorderColor[0] == 1.0f)
			{
				staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
			}
			else
			{
				staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
			}
		}
		else
		{
			staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		}
	}
}

void RootSignature::Finalize(D3D12_ROOT_SIGNATURE_FLAGS flags)
{
	if (mFinalized)
	{
		return;
	}

	assert(mNumInitializedStaticSamplers == mNumSamplers);

	D3D12_ROOT_SIGNATURE_DESC desc;
	desc.NumParameters = mNumParameters;
	desc.pParameters = (const D3D12_ROOT_PARAMETER*)mParamArray;
	desc.NumStaticSamplers = mNumSamplers;
	desc.pStaticSamplers = (const D3D12_STATIC_SAMPLER_DESC*)mSamplerArray;
	desc.Flags = flags;

	mDescriptorTableBitMap = 0;
	mSamplerTableBitMap = 0;

	ID3DBlob* outBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &outBlob, &errorBlob);
	if (errorBlob != NULL)
	{
		OutputDebugString((LPCTSTR)errorBlob->GetBufferPointer());
		errorBlob->Release();
	}
	mDevice->CreateRootSignature(1, outBlob->GetBufferPointer(), outBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&mRootSignature);

	mFinalized = true;
}