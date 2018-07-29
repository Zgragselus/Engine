#include "Texture.h"

using namespace Engine;

void Texture::Init(D3DRenderer* renderer, size_t width, size_t height, size_t depth, Graphics::Format format, const void* data, DescriptorHeap* heap, bool createUAV)
{
	mRenderer = renderer;
	mMiplevels = 1;

	if (createUAV)
	{
		mUAV = new DescriptorHandle[1];
	}

	mUsageState = D3D12_RESOURCE_STATE_COPY_DEST;

	D3D12_RESOURCE_DESC texDesc;
	texDesc.Alignment = 0;
	if (depth == 1)
	{
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	}
	else
	{
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	}
	texDesc.Width = width;
	texDesc.Height = (unsigned int)height;
	texDesc.DepthOrArraySize = (UINT16)depth;
	texDesc.MipLevels = 1;
	texDesc.Format = Graphics::GetFormat(format);
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	if (createUAV)
	{
		texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}
	else
	{
		texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	}

	D3D12_HEAP_PROPERTIES heapProps;
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	renderer->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &texDesc, mUsageState, NULL, __uuidof(ID3D12Resource), (void**)&mResource);

	D3D12_SUBRESOURCE_DATA texResource;
	texResource.pData = data;
	texResource.RowPitch = width * Graphics::GetFormatSize(format);
	texResource.SlicePitch = texResource.RowPitch * height;

	if (data)
	{
		renderer->GetGraphicsContext()->InitializeTexture(this, 1, &texResource);
	}

	if (heap == nullptr)
	{
		mCpuDesc = renderer->Heap()->Allocate();
		if (createUAV)
		{
			mUAV[0] = renderer->Heap()->Allocate();
		}
	}
	else
	{
		mCpuDesc = heap->Allocate();
		if (createUAV)
		{
			mUAV[0] = heap->Allocate();
		}
	}
	renderer->GetDevice()->CreateShaderResourceView(mResource, NULL, mCpuDesc.mCpuHandle);

	if (createUAV)
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
		uavDesc.Format = Graphics::GetFormat(format);
		uavDesc.Texture3D.FirstWSlice = 0;
		uavDesc.Texture3D.MipSlice = 0;
		uavDesc.Texture3D.WSize = (UINT)depth;

		renderer->GetDevice()->CreateUnorderedAccessView(mResource, NULL, &uavDesc, mUAV[0].mCpuHandle);
	}
}

void Texture::InitMipmaps(D3DRenderer* renderer, size_t width, size_t height, size_t depth, size_t miplevels, Graphics::Format format, const void** data, DescriptorHeap* heap, bool createUAV)
{
	mRenderer = renderer;
	mMiplevels = (unsigned int)miplevels;

	if (createUAV)
	{
		mUAV = new DescriptorHandle[miplevels];
	}

	mUsageState = D3D12_RESOURCE_STATE_COPY_DEST;

	D3D12_RESOURCE_DESC texDesc;
	texDesc.Alignment = 0;
	if (depth == 1)
	{
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	}
	else
	{
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	}
	texDesc.Width = width;
	texDesc.Height = (unsigned int)height;
	texDesc.DepthOrArraySize = (UINT16)depth;
	texDesc.MipLevels = (UINT16)miplevels;
	texDesc.Format = Graphics::GetFormat(format);
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	if (createUAV)
	{
		texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}
	else
	{
		texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	}

	D3D12_HEAP_PROPERTIES heapProps;
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	renderer->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &texDesc, mUsageState, NULL, __uuidof(ID3D12Resource), (void**)&mResource);

	if (data)
	{
		D3D12_SUBRESOURCE_DATA* texResource = new D3D12_SUBRESOURCE_DATA[miplevels];
		size_t w = width;
		size_t h = height;
		for (size_t i = 0; i < miplevels; i++)
		{
			texResource[i].pData = data[i];
			texResource[i].RowPitch = w * Graphics::GetFormatSize(format);
			texResource[i].SlicePitch = texResource[i].RowPitch * h;

			w /= 2;
			h /= 2;
		}

		renderer->GetGraphicsContext()->InitializeTexture(this, (unsigned int)miplevels, texResource);

		delete[] texResource;
	}

	if (heap == nullptr)
	{
		mCpuDesc = renderer->Heap()->Allocate();
		if (createUAV)
		{
			for (size_t i = 0; i < miplevels; i++)
			{
				mUAV[i] = renderer->Heap()->Allocate();
			}
		}
	}
	else
	{
		mCpuDesc = heap->Allocate();
		if (createUAV)
		{
			for (size_t i = 0; i < miplevels; i++)
			{
				mUAV[i] = heap->Allocate();
			}
		}
	}
	renderer->GetDevice()->CreateShaderResourceView(mResource, NULL, mCpuDesc.mCpuHandle);

	if (createUAV)
	{
		size_t slices = depth;
		for (size_t i = 0; i < miplevels; i++)
		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
			uavDesc.Format = Graphics::GetFormat(format);
			uavDesc.Texture3D.FirstWSlice = 0;
			uavDesc.Texture3D.MipSlice = (UINT)i;
			uavDesc.Texture3D.WSize = (UINT)slices;
			
			renderer->GetDevice()->CreateUnorderedAccessView(mResource, NULL, &uavDesc, mUAV[i].mCpuHandle);

			slices /= 2;
		}
	}
}