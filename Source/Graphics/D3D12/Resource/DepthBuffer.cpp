#include "DepthBuffer.h"
#include "../D3DRenderer.h"

using namespace Engine;

void DepthBuffer::CreateDerivedViews(DXGI_FORMAT format)
{
	// Create Default and Depth-Read views
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = GetDSVFormat(format);
	if (mIsCubemap)
	{
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.Texture2DArray.ArraySize = 1;
		dsvDesc.Texture2DArray.MipSlice = 0;

		for (int i = 0; i < 6; i++)
		{
			mDSVCube[i] = mRenderer->HeapDSV()->Allocate();

			dsvDesc.Texture2DArray.FirstArraySlice = i;

			mRenderer->GetDevice()->CreateDepthStencilView(mResource, &dsvDesc, mDSVCube[i].mCpuHandle);
		}
	}
	else
	{
		if (mFragmentCount > 1)
		{
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
		}

		if (!mDSV[0].IsValid())
		{
			mDSV[0] = mRenderer->HeapDSV()->Allocate();
			mDSV[1] = mRenderer->HeapDSV()->Allocate();
		}

		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		mRenderer->GetDevice()->CreateDepthStencilView(mResource, &dsvDesc, mDSV[0].mCpuHandle);

		dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
		mRenderer->GetDevice()->CreateDepthStencilView(mResource, &dsvDesc, mDSV[1].mCpuHandle);

		// Create Stencil-Read and Depth-Stencil Read views
		DXGI_FORMAT stencilFormat = GetStencilFormat(format);
		if (stencilFormat != DXGI_FORMAT_UNKNOWN)
		{
			if (!mDSV[2].IsValid())
			{
				mDSV[2] = mRenderer->HeapDSV()->Allocate();
				mDSV[3] = mRenderer->HeapDSV()->Allocate();
			}

			dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_STENCIL;
			mRenderer->GetDevice()->CreateDepthStencilView(mResource, &dsvDesc, mDSV[2].mCpuHandle);

			dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH | D3D12_DSV_FLAG_READ_ONLY_STENCIL;
			mRenderer->GetDevice()->CreateDepthStencilView(mResource, &dsvDesc, mDSV[3].mCpuHandle);
		}
		else
		{
			mDSV[2] = mDSV[0];
			mDSV[3] = mDSV[1];
		}

		// Create shader resource views (depth and stencil)
		if (!mDepthSRV.IsValid())
		{
			mDepthSRV = mRenderer->Heap()->Allocate();
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = GetDepthFormat(format);
		if (mFragmentCount > 1)
		{
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;
		}
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		mRenderer->GetDevice()->CreateShaderResourceView(mResource, &SRVDesc, mDepthSRV.mCpuHandle);

		if (stencilFormat != DXGI_FORMAT_UNKNOWN)
		{
			if (!mStencilSRV.IsValid())
			{
				mStencilSRV = mRenderer->Heap()->Allocate();
			}

			SRVDesc.Format = stencilFormat;
			mRenderer->GetDevice()->CreateShaderResourceView(mResource, &SRVDesc, mDepthSRV.mCpuHandle);
		}
	}
}

void DepthBuffer::Create(unsigned int width, unsigned int height, Graphics::Format format, bool cubemap, unsigned int msaaSamples, unsigned int msaaQuality)
{
	mFragmentCount = msaaSamples;
	mIsCubemap = cubemap;

	D3D12_RESOURCE_DESC desc = DescTex2D(width, height, mIsCubemap ? 6 : 1, 1, format, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	desc.SampleDesc.Count = mFragmentCount;
	desc.SampleDesc.Quality = 0;

	D3D12_CLEAR_VALUE clear;
	clear.Format = Graphics::GetFormat(format);
	clear.DepthStencil.Depth = mClearDepth;
	clear.DepthStencil.Stencil = mClearStencil;

	CreateTextureResource(mRenderer->GetDevice(), desc, clear);

	CreateDerivedViews(Graphics::GetFormat(format));
}

void DepthBuffer::Release(bool dtor)
{
	if (!dtor)
	{
		mResource->Release();
	}

	if (mDSV[0].mCpuHandle.ptr != 0)
	{
		mRenderer->HeapDSV()->Free(mDSV[0]);
		mRenderer->HeapDSV()->Free(mDSV[1]);
		mRenderer->HeapDSV()->Free(mDSV[2]);
		mRenderer->HeapDSV()->Free(mDSV[3]);
		mRenderer->Heap()->Free(mDepthSRV);
		mRenderer->Heap()->Free(mStencilSRV);
	}
}