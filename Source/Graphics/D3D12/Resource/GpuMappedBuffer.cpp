#include "GpuMappedBuffer.h"

using namespace Engine;

GpuMappedBuffer::GpuMappedBuffer()
{

}

GpuMappedBuffer::~GpuMappedBuffer()
{
	if (mResource != nullptr)
	{
		if (mCpuVirtualAddress != nullptr)
		{
			Unmap();
		}

		mResource->Release();
		mResource = nullptr;
		mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
	}
}

void GpuMappedBuffer::Init(D3DRenderer* renderer, unsigned int numElements, unsigned int elementSize, enum Mode mode)
{
	D3D12_HEAP_PROPERTIES heapProps;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;
	heapProps.Type = mode == UPLOAD ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_READBACK;

	D3D12_RESOURCE_DESC resDesc;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = 0;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Width = (size_t)(numElements * elementSize);
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	mUsageState = mode == UPLOAD ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COPY_DEST;

	renderer->GetDevice()->CreateCommittedResource(&heapProps,
		D3D12_HEAP_FLAG_NONE, 
		&resDesc, 
		mUsageState,
		nullptr,
		__uuidof(ID3D12Resource),
		(void**)&mResource);

	mGpuVirtualAddress = mResource->GetGPUVirtualAddress();
	mCpuVirtualAddress = nullptr;
}

void* GpuMappedBuffer::Map(size_t begin, size_t end)
{
	assert(mCpuVirtualAddress == nullptr);
	if (begin == 0 && end == 0)
	{
		mResource->Map(0, nullptr, &mCpuVirtualAddress);
	}
	else
	{
		D3D12_RANGE range;
		range.Begin = begin;
		range.End = end;
		mResource->Map(0, &range, &mCpuVirtualAddress);
	}
	return mCpuVirtualAddress;
}

void GpuMappedBuffer::Unmap()
{
	assert(mCpuVirtualAddress != nullptr);
	mResource->Unmap(0, nullptr);
	mCpuVirtualAddress = nullptr;
}