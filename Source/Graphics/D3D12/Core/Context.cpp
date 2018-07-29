#include "Context.h"
#include <cassert>
#include "../Resource/GpuBuffer.h"

using namespace Engine;

#define VALID_COMPUTE_QUEUE_RESOURCE_STATES \
	( D3D12_RESOURCE_STATE_UNORDERED_ACCESS \
	| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE \
	| D3D12_RESOURCE_STATE_COPY_DEST \
	| D3D12_RESOURCE_STATE_COPY_SOURCE )

Context* ContextManager::AllocateContext(ID3D12Device* device, CommandAllocator::CommandListType type)
{
	std::lock_guard<std::mutex> lockGuard(mContextAllocationMutex);
	auto& availContext = mAvailableContexts[type];
	Context* ret = nullptr;
	if (availContext.empty())
	{
		ret = new Context(device, mOwnerManager, this, type);
		mContextPool[type].emplace_back(ret);
	}
	else
	{
		ret = mAvailableContexts[type].front();
		ret->mCmdAllocator = mOwnerManager->GetQueue(ret->mType)->RequestAllocator();
		mAvailableContexts[type].pop();
	}

	return ret;
}

void ContextManager::FreeContext(Context* context)
{
	std::lock_guard<std::mutex> lockGuard(mContextAllocationMutex);
	mAvailableContexts[context->mType].push(context);
}

void ContextManager::DestroyAllContexts()
{
	for (unsigned int i = 0; i < 4; i++)
	{
		for (Context* c : mContextPool[i])
		{
			delete c;
		}

		mContextPool[i].clear();
	}
}

Context::Context(ID3D12Device* device, CommandListManager* owner, ContextManager* ctxOwner, CommandAllocator::CommandListType type)
{
	mDevice = device;
	mOwningContext = ctxOwner;
	mOwningManager = owner;
	mOwningManager->CreateNewCommandList(type, &mCmdList, &mCmdAllocator);
	mGpuAllocator = new LinearAllocator(device, owner, LinearAllocatorType::GPU_EXCLUSIVE);
	mCpuAllocator = new LinearAllocator(device, owner, LinearAllocatorType::CPU_WRITABLE);
	mReadbackAllocator = new LinearAllocator(device, owner, LinearAllocatorType::CPU_READABLE);
	mType = type;
}

Context::~Context()
{
	if (mCmdList)
	{
		delete mCmdList;
	}

	delete mCpuAllocator;
	delete mReadbackAllocator;
	delete mGpuAllocator;
}

void Context::BindDescriptorHeaps()
{
	unsigned int nonNullHeaps = 0;
	ID3D12DescriptorHeap* heapsToBind[DescriptorHeap::DESCRIPTOR_HEAP_TYPE_COUNT];
	for (unsigned int i = 0; i < DescriptorHeap::DESCRIPTOR_HEAP_TYPE_COUNT; i++)
	{
		if (mCurrentDescriptorHeaps[i])
		{
			ID3D12DescriptorHeap* it = mCurrentDescriptorHeaps[i]->GetHeap();
			if (it != nullptr)
			{
				heapsToBind[nonNullHeaps++] = it;
			}
		}		
	}

	if (nonNullHeaps > 0)
	{
		mCmdList->Get()->SetDescriptorHeaps(nonNullHeaps, heapsToBind);
	}
}

void Context::Begin(PipelineState* state)
{
	mCmdAllocator->Get()->Reset();

	if (state)
	{
		mCmdList->Get()->Reset(mCmdAllocator->Get(), state->Get());
	}
	else
	{
		mCmdList->Get()->Reset(mCmdAllocator->Get(), NULL);
	}

	mGraphicsRootSignature = nullptr;
	mGraphicsPipelineState = nullptr;
	mComputeRootSignature = nullptr;
	mComputePipelineState = nullptr;
	mBarriersToFlush = 0;

	BindDescriptorHeaps();
}

uint64_t Context::Flush()
{
	FlushResourceBarriers();

	uint64_t fenceValue = mOwningManager->GetQueue(mType)->ExecuteCommandList(mCmdList);

	mOwningManager->WaitForFence(fenceValue);

	mCmdList->Get()->Reset(mCmdAllocator->Get(), nullptr);

	return fenceValue;
}

uint64_t Context::Finish()
{
	FlushResourceBarriers();

	CommandQueue* queue = mOwningManager->GetQueue(mType);

	uint64_t fenceValue = queue->ExecuteCommandList(mCmdList);
	queue->DiscardAllocator(fenceValue, mCmdAllocator);
	mCmdAllocator = nullptr;

	mCpuAllocator->CleanupUsedPages(fenceValue);
	mReadbackAllocator->CleanupUsedPages(fenceValue);
	mGpuAllocator->CleanupUsedPages(fenceValue);

	mOwningManager->WaitForFence(fenceValue);

	mOwningContext->FreeContext(this);

	return fenceValue;
}

uint64_t Context::GetTimestampFrequency()
{
	uint64_t freq;
	CommandQueue* queue = mOwningManager->GetQueue(mType);
	queue->GetCommandQueue()->GetTimestampFrequency(&freq);
	return freq;
}

void Context::CopyBuffer(GpuResource* dest, GpuResource* src)
{
	TransitionResource(dest, D3D12_RESOURCE_STATE_COPY_DEST);
	TransitionResource(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
	FlushResourceBarriers();
	mCmdList->Get()->CopyResource(dest->Get(), src->Get());
}

void Context::CopyBufferRegion(GpuResource* dest, size_t destOffset, GpuResource* src, size_t srcOffset, size_t numBytes)
{
	TransitionResource(dest, D3D12_RESOURCE_STATE_COPY_DEST);
	//TransitionResource(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
	FlushResourceBarriers();
	mCmdList->Get()->CopyBufferRegion(dest->Get(), destOffset, src->Get(), srcOffset, numBytes);
}

void Context::CopySubresource(GpuResource* dest, unsigned int destSubIndex, GpuResource* src, unsigned int srcSubIndex)
{

}

void Context::CopyCounter(GpuResource* dest, size_t destOffset, StructuredBuffer* src, size_t bytes)
{
	TransitionResource(dest, D3D12_RESOURCE_STATE_COPY_DEST);
	TransitionResource(&src->GetCounterBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE);
	FlushResourceBarriers();
	mCmdList->Get()->CopyBufferRegion(dest->Get(), destOffset, src->GetCounterBuffer().Get(), 0, bytes);
}

void Context::CopyCounter(GpuResource* dest, size_t destOffset, ByteAddressBuffer* src, size_t bytes)
{
	TransitionResource(dest, D3D12_RESOURCE_STATE_COPY_DEST);
	TransitionResource(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
	FlushResourceBarriers();
	mCmdList->Get()->CopyBufferRegion(dest->Get(), destOffset, src->Get(), 0, bytes);
}

void Context::ResetCounter(StructuredBuffer* buffer, uint32_t value, size_t bytes)
{
	FillBuffer(&buffer->GetCounterBuffer(), 0, value, bytes);
	TransitionResource(&buffer->GetCounterBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

void Context::ResetCounter(ByteAddressBuffer* buffer, uint32_t value, size_t bytes)
{
	FillBuffer(buffer, 0, value, bytes);
	TransitionResource(buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

void Context::InitializeBuffer(GpuResource* buffer, const void* data, size_t size, size_t offset)
{
	Begin();

	DynAlloc mem = ReserveUploadMemory(size);
	memcpy(mem.mDataPtr, data, size);

	TransitionResource(buffer, D3D12_RESOURCE_STATE_COPY_DEST, true);
	mCmdList->Get()->CopyBufferRegion(buffer->Get(), offset, mem.mBuffer.Get(), 0, size);
	TransitionResource(buffer, D3D12_RESOURCE_STATE_GENERIC_READ, true);

	Finish();
}

void Context::InitializeComputeBuffer(GpuResource* buffer, const void* data, size_t size, size_t offset)
{
	Begin();

	DynAlloc mem = ReserveUploadMemory(size);
	memcpy(mem.mDataPtr, data, size);

	TransitionResource(buffer, D3D12_RESOURCE_STATE_COPY_DEST, true);
	mCmdList->Get()->CopyBufferRegion(buffer->Get(), offset, mem.mBuffer.Get(), 0, size);
	TransitionResource(buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);

	Finish();
}

void Context::InitializeTexture(GpuResource* texture, unsigned int numSubresources, D3D12_SUBRESOURCE_DATA data[])
{
	size_t uploadBufferSize = GetRequiredIntermediateSize(texture->Get(), 0, numSubresources);

	Begin();

	DynAlloc mem = ReserveUploadMemory(uploadBufferSize);
	UpdateSubresources(mCmdList, texture->Get(), mem.mBuffer.Get(), 0, 0, numSubresources, data);
	TransitionResource(texture, D3D12_RESOURCE_STATE_GENERIC_READ, true);

	Finish();
}

void Context::ReadbackBuffer(GpuResource* buffer, void* data, size_t size, size_t offset)
{
	Begin();

	DynAlloc mem = ReserveReadbackMemory(size);

	TransitionResource(buffer, D3D12_RESOURCE_STATE_COPY_SOURCE, true);
	mCmdList->Get()->CopyBufferRegion(mem.mBuffer.Get(), 0, buffer->Get(), 0, size);
	TransitionResource(buffer, D3D12_RESOURCE_STATE_GENERIC_READ, true);

	memcpy(data, mem.mDataPtr, size);

	Finish();
}

void Context::ReadbackTexture2D(GpuResource* readbackBuffer, PixelBuffer* srcBuffer, uint32_t miplevel)
{
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedFootprint;
	mDevice->GetCopyableFootprints(&srcBuffer->Get()->GetDesc(), miplevel, 1, 0, &placedFootprint, nullptr, nullptr, nullptr);

	Begin();

	D3D12_TEXTURE_COPY_LOCATION src;
	src.pResource = srcBuffer->Get();
	src.SubresourceIndex = miplevel;
	src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

	D3D12_TEXTURE_COPY_LOCATION dest;
	dest.pResource = readbackBuffer->Get();
	dest.PlacedFootprint = placedFootprint;
	dest.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

	TransitionResource(srcBuffer, D3D12_RESOURCE_STATE_COPY_SOURCE, true);
	mCmdList->Get()->CopyTextureRegion(&dest, 0, 0, 0, &src, nullptr);

	Finish();
}

void Context::WriteBuffer(GpuResource* dest, size_t destOffset, const void* data, size_t numBytes)
{
	DynAlloc TempSpace = mCpuAllocator->Allocate(numBytes, 512);
	memcpy(TempSpace.mDataPtr, data, numBytes);
	CopyBufferRegion(dest, destOffset, &TempSpace.mBuffer, TempSpace.mOffset, numBytes);
}

void Context::FillBuffer(GpuResource* dest, size_t destOffset, DWParam value, size_t numBytes)
{
	DynAlloc TempSpace = mCpuAllocator->Allocate(numBytes, 512);
	DWParam* ptr = (DWParam*)TempSpace.mDataPtr;
	for (size_t i = 0; i < numBytes / sizeof(DWParam); i++)
	{
		*ptr = value;
		ptr++;
	}
	CopyBufferRegion(dest, destOffset, &TempSpace.mBuffer, TempSpace.mOffset, numBytes);
}

void Context::TransitionResource(GpuResource* resource, D3D12_RESOURCE_STATES newState, bool flushImmediate)
{
	D3D12_RESOURCE_STATES prevState = resource->mUsageState;

	if (mType == D3D12_COMMAND_LIST_TYPE_COMPUTE)
	{
		assert((prevState & VALID_COMPUTE_QUEUE_RESOURCE_STATES) == prevState);
		assert((newState & VALID_COMPUTE_QUEUE_RESOURCE_STATES) == newState);
	}

	if (prevState != newState)
	{
		assert(mBarriersToFlush < 16);
		D3D12_RESOURCE_BARRIER& desc = mResourceBarriers[mBarriersToFlush++];

		desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		desc.Transition.pResource = resource->Get();
		desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		desc.Transition.StateBefore = prevState;
		desc.Transition.StateAfter = newState;

		// Check to see if we already started the transition
		if (newState == resource->mTransitionState)
		{
			desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
			resource->mTransitionState = (D3D12_RESOURCE_STATES)-1;
		}
		else
		{
			desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		}

		resource->mUsageState = newState;
	}
	else if (newState == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	{
		InsertUAVBarrier(resource, flushImmediate);
	}

	if (flushImmediate || mBarriersToFlush == 16)
	{
		FlushResourceBarriers();
	}
}

void Context::BeginResourceTransition(GpuResource* resource, D3D12_RESOURCE_STATES newState, bool flushImmediate)
{
	if (resource->mTransitionState != (D3D12_RESOURCE_STATES)-1)
	{
		TransitionResource(resource, resource->mTransitionState);
	}

	D3D12_RESOURCE_STATES prevState = resource->mUsageState;

	if (prevState != newState)
	{
		assert(mBarriersToFlush < 16);
		D3D12_RESOURCE_BARRIER& desc = mResourceBarriers[mBarriersToFlush++];

		desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		desc.Transition.pResource = resource->Get();
		desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		desc.Transition.StateBefore = prevState;
		desc.Transition.StateAfter = newState;

		desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY;

		resource->mTransitionState = newState;
	}

	if (flushImmediate || mBarriersToFlush == 16)
	{
		FlushResourceBarriers();
	}
}

void Context::InsertUAVBarrier(GpuResource* resource, bool flushImmediate)
{
	assert(mBarriersToFlush < 16);
	D3D12_RESOURCE_BARRIER& desc = mResourceBarriers[mBarriersToFlush++];

	desc.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	desc.UAV.pResource = resource->Get();

	if (flushImmediate)
	{
		FlushResourceBarriers();
	}
}

void Context::InsertAliasBarrier(GpuResource* before, GpuResource* after, bool flushImmediate)
{
	assert(mBarriersToFlush < 16);
	D3D12_RESOURCE_BARRIER& desc = mResourceBarriers[mBarriersToFlush++];

	desc.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
	desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	desc.Aliasing.pResourceBefore = before->Get();
	desc.Aliasing.pResourceAfter = after->Get();

	if (flushImmediate)
	{
		FlushResourceBarriers();
	}
}

void Context::FlushResourceBarriers()
{
	if (mBarriersToFlush > 0)
	{
		mCmdList->Get()->ResourceBarrier(mBarriersToFlush, mResourceBarriers);
		mBarriersToFlush = 0;
	}
}

GraphicsContext::GraphicsContext(ID3D12Device* device, CommandListManager* owner, ContextManager* ctxOwner) : Context(device, owner, ctxOwner, CommandAllocator::CommandListType::COMMAND_LIST_DIRECT)
{

}

GraphicsContext::~GraphicsContext()
{

}

void GraphicsContext::SetBufferSRV(unsigned int rootIndex, const GpuBuffer& SRV, uint64_t offset)
{
	mCmdList->Get()->SetGraphicsRootShaderResourceView(rootIndex, SRV.GetGpuVirtualAddress() + offset);
}

void GraphicsContext::SetBufferUAV(unsigned int rootIndex, const GpuBuffer& UAV, uint64_t offset)
{
	mCmdList->Get()->SetGraphicsRootUnorderedAccessView(rootIndex, UAV.GetGpuVirtualAddress() + offset);
}