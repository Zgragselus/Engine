#include "CommandQueue.h"

using namespace Engine;

uint64_t CommandQueue::ExecuteCommandList(CommandList* list)
{
	std::lock_guard<std::mutex> guard(mFenceMutex);
	
	list->Get()->Close();

	// Execute cmd list
	ID3D12CommandList* ptr[1];
	ptr[0] = (ID3D12CommandList*)list->Get();
	mQueue->ExecuteCommandLists(1, ptr);

	// Signal next fence
	mQueue->Signal(mFence, mNextFenceValue);

	// Increment fence value
	return mNextFenceValue++;
}

CommandAllocator* CommandQueue::RequestAllocator()
{
	uint64_t completedFence = mFence->GetCompletedValue();
	return mAllocatorPool->RequestAllocator(completedFence);
}

void CommandQueue::DiscardAllocator(uint64_t fenceValueForReset, CommandAllocator* allocator)
{
	mAllocatorPool->DiscardAllocator(fenceValueForReset, allocator);
}

CommandQueue::CommandQueue(ID3D12Device* device, CommandListManager* manager, CommandAllocator::CommandListType type)
{
	mType = type;
	mManager = manager;
	mNextFenceValue = ((uint64_t)mType << 56 | 1);
	mLastCompletedFenceValue = ((uint64_t)mType << 56);

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = CommandAllocator::CommandListTypeD3D[type];
	desc.NodeMask = 1;
	device->CreateCommandQueue(&desc, __uuidof(ID3D12CommandQueue), (void**)&mQueue);

	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&mFence);
	mFence->Signal((uint64_t)mType << 56);

	mFenceEventHandle = CreateEvent(nullptr, false, false, nullptr);

	mAllocatorPool = new CommandAllocatorPool(device, mType);
}

CommandQueue::~CommandQueue()
{
	delete mAllocatorPool;
}

uint64_t CommandQueue::IncrementFence()
{
	std::lock_guard<std::mutex> guard(mFenceMutex);
	mQueue->Signal(mFence, mNextFenceValue);
	return mNextFenceValue++;
}

bool CommandQueue::IsFenceCompleted(uint64_t fenceValue)
{
	if (fenceValue > mLastCompletedFenceValue)
	{
		mLastCompletedFenceValue = std::max(mLastCompletedFenceValue, mFence->GetCompletedValue());
	}

	return fenceValue <= mLastCompletedFenceValue;
}

void CommandQueue::StallForFence(uint64_t fenceValue)
{
	CommandQueue* producer = mManager->GetQueue((CommandAllocator::CommandListType)(fenceValue >> 56));
	mQueue->Wait(producer->mFence, fenceValue);
}

void CommandQueue::StallForProducer(CommandQueue& producer)
{
	mQueue->Wait(producer.mFence, producer.mNextFenceValue - 1);
}

void CommandQueue::WaitForFence(uint64_t fenceValue)
{
	if (IsFenceCompleted(fenceValue))
	{
		return;
	}
	else
	{
		std::lock_guard<std::mutex> guard(mEventMutex);
		mFence->SetEventOnCompletion(fenceValue, mFenceEventHandle);
		WaitForSingleObject(mFenceEventHandle, INFINITE);
		mLastCompletedFenceValue = fenceValue;
	}
}

CommandListManager::CommandListManager(ID3D12Device* device)
{
	mDevice = device;
	mGraphicsQueue = new CommandQueue(mDevice, this, CommandAllocator::COMMAND_LIST_DIRECT);
	mComputeQueue = new CommandQueue(mDevice, this, CommandAllocator::COMMAND_LIST_COMPUTE);
	mCopyQueue = new CommandQueue(mDevice, this, CommandAllocator::COMMAND_LIST_COPY);
}

CommandListManager::~CommandListManager()
{
	delete mGraphicsQueue;
	delete mComputeQueue;
	delete mCopyQueue;
}

void CommandListManager::CreateNewCommandList(CommandAllocator::CommandListType type, CommandList** list, CommandAllocator** allocator)
{
	switch (type)
	{
	case CommandAllocator::COMMAND_LIST_DIRECT:
		*allocator = mGraphicsQueue->RequestAllocator();
		break;

	case CommandAllocator::COMMAND_LIST_COMPUTE:
		*allocator = mComputeQueue->RequestAllocator();
		break;

	case CommandAllocator::COMMAND_LIST_COPY:
		*allocator = mCopyQueue->RequestAllocator();
		break;

	default:
		break;
	}

	*list = new CommandList(mDevice, *allocator, type);
}

void CommandListManager::WaitForFence(uint64_t fenceValue)
{
	CommandQueue* producer = GetQueue((CommandAllocator::CommandListType)(fenceValue >> 56));
	producer->WaitForFence(fenceValue);
}