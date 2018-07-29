#include "CommandAllocator.h"

using namespace Engine;

D3D12_COMMAND_LIST_TYPE CommandAllocator::CommandListTypeD3D[4] =
{
	D3D12_COMMAND_LIST_TYPE_DIRECT,
	D3D12_COMMAND_LIST_TYPE_COMPUTE,
	D3D12_COMMAND_LIST_TYPE_BUNDLE,
	D3D12_COMMAND_LIST_TYPE_COPY
};

CommandAllocatorPool::CommandAllocatorPool(ID3D12Device* device, CommandAllocator::CommandListType type)
{
	mType = type;
	mDevice = device;
}

CommandAllocatorPool::~CommandAllocatorPool()
{
	for (size_t i = 0; i < mAllocatorPool.size(); i++)
	{
		delete mAllocatorPool[i];
	}

	mAllocatorPool.clear();
}

CommandAllocator* CommandAllocatorPool::RequestAllocator(uint64_t completedFenceValue)
{
	std::lock_guard<std::mutex> guard(mAllocatorMutex);

	CommandAllocator* allocator = nullptr;

	if (!mReadyAllocators.empty())
	{
		std::pair<uint64_t, CommandAllocator*>& allocPair = mReadyAllocators.front();
		if (allocPair.first <= completedFenceValue)
		{
			allocator = allocPair.second;
			allocator->Get()->Reset();
			mReadyAllocators.pop();
		}
	}

	if (allocator == nullptr)
	{
		allocator = new CommandAllocator(mDevice, mType);
		mAllocatorPool.push_back(allocator);
	}

	return allocator;
}

void CommandAllocatorPool::DiscardAllocator(uint64_t fenceValue, CommandAllocator* allocator)
{
	std::lock_guard<std::mutex> guard(mAllocatorMutex);
	mReadyAllocators.push(std::make_pair(fenceValue, allocator));
}