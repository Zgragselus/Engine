#include "ColorBuffer.h"
#include "../D3DRenderer.h"

using namespace Engine;

void ColorBuffer::CreateDerivedViews(ID3D12Device* device, DXGI_FORMAT format, unsigned int arraySize, unsigned int numMips)
{
	// Disallow auto mip map generation on texture arrays
	assert(numMips == 1 || arraySize == 1);
	
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	rtvDesc.Format = format;
	uavDesc.Format = GetUAVFormat(format);
	srvDesc.Format = format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	if (mIsCubemap)
	{
		if (!mSRV.IsValid())
		{
			mRTVCube[0] = mRenderer->HeapRTV()->Allocate();
			mRTVCube[1] = mRenderer->HeapRTV()->Allocate();
			mRTVCube[2] = mRenderer->HeapRTV()->Allocate();
			mRTVCube[3] = mRenderer->HeapRTV()->Allocate();
			mRTVCube[4] = mRenderer->HeapRTV()->Allocate();
			mRTVCube[5] = mRenderer->HeapRTV()->Allocate();
			mSRV = mRenderer->Heap()->Allocate();
		}

		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = 1;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

		// Create shader resource view
		mRenderer->GetDevice()->CreateShaderResourceView(mResource, &srvDesc, mSRV.mCpuHandle);

		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Texture2DArray.ArraySize = 1;
		rtvDesc.Texture2DArray.MipSlice = 0;
		rtvDesc.Texture2DArray.PlaneSlice = 0;

		for (int i = 0; i < 6; i++)
		{
			rtvDesc.Texture2DArray.FirstArraySlice = i;

			// Create render target view
			mRenderer->GetDevice()->CreateRenderTargetView(mResource, &rtvDesc, mRTVCube[i].mCpuHandle);
		}
	}
	else
	{
		if (mArraySize > 1)
		{
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.Texture2DArray.MipSlice = 0;
			rtvDesc.Texture2DArray.PlaneSlice = 0;
			rtvDesc.Texture2DArray.FirstArraySlice = 0;
			rtvDesc.Texture2DArray.ArraySize = mArraySize;

			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			uavDesc.Texture2DArray.MipSlice = 0;
			uavDesc.Texture2DArray.FirstArraySlice = 0;
			uavDesc.Texture2DArray.ArraySize = (UINT)mArraySize;

			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.MipLevels = mNumMipMaps;
			srvDesc.Texture2DArray.MostDetailedMip = 0;
			srvDesc.Texture2DArray.FirstArraySlice = 0;
			srvDesc.Texture2DArray.ArraySize = (UINT)mArraySize;
		}
		else if (mFragmentCount > 1)
		{
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;

			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.MipSlice = 0;

			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = mNumMipMaps;
			srvDesc.Texture2D.MostDetailedMip = 0;
		}

		if (!mSRV.IsValid())
		{
			mRTV = mRenderer->HeapRTV()->Allocate();
			mSRV = mRenderer->Heap()->Allocate();
		}

		// Create render target view
		mRenderer->GetDevice()->CreateRenderTargetView(mResource, &rtvDesc, mRTV.mCpuHandle);

		// Create shader resource view
		mRenderer->GetDevice()->CreateShaderResourceView(mResource, &srvDesc, mSRV.mCpuHandle);

		if (mFragmentCount > 1)
		{
			return;
		}

		// For non-MSAA create unordered access view (RWTexture2D) for each miplevel
		for (unsigned int i = 0; i < mNumMipMaps; ++i)
		{
			if (!mUAV[i].IsValid())
			{
				mUAV[i] = mRenderer->Heap()->Allocate();
			}

			mRenderer->GetDevice()->CreateUnorderedAccessView(mResource, nullptr, &uavDesc, mUAV[i].mCpuHandle);

			uavDesc.Texture2D.MipSlice++;
		}
	}
}

void ColorBuffer::CreateFromSwapChain(IDXGISwapChain3* swapChain, unsigned int buffer)
{
	mUsageState = D3D12_RESOURCE_STATE_PRESENT;
	swapChain->GetBuffer(buffer, __uuidof(ID3D12Resource), (void**)&mResource);

	mRTV = mRenderer->HeapRTV()->Allocate();
	mRenderer->GetDevice()->CreateRenderTargetView(mResource, NULL, mRTV.mCpuHandle);
}

void ColorBuffer::Create(unsigned int width, unsigned int height, unsigned int numMips, Graphics::Format format, bool cubemap, unsigned int msaaSamples, unsigned int msaaQuality, D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr)
{
	mNumMipMaps = (numMips == 0 ? ComputeNumMips(width, height) : numMips);
	mIsCubemap = cubemap;

	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if (msaaSamples == 1)
	{
		flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	mFragmentCount = msaaSamples;

	D3D12_RESOURCE_DESC desc = DescTex2D(width, height, mIsCubemap ? 6 : 1, mNumMipMaps, format, flags);
	desc.SampleDesc.Count = mFragmentCount;
	desc.SampleDesc.Quality = 0;

	D3D12_CLEAR_VALUE clear = {};
	clear.Format = Graphics::GetFormat(format);
	clear.Color[0] = mClearColor[0];
	clear.Color[1] = mClearColor[1];
	clear.Color[2] = mClearColor[2];
	clear.Color[3] = mClearColor[3];

	CreateTextureResource(mRenderer->GetDevice(), desc, clear);
	CreateDerivedViews(mRenderer->GetDevice(), Graphics::GetFormat(format), mArraySize, mNumMipMaps);
}

void ColorBuffer::Release(bool dtor)
{
	if (!dtor)
	{
		mResource->Release();
	}

	mRenderer->HeapRTV()->Free(mRTV);
}