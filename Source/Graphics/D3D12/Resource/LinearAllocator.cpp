#include "LinearAllocator.h"
#include "../Core/CommandQueue.h"

using namespace Engine;

LinearAllocatorPageManager::LinearAllocatorPageManager(ID3D12Device* device, CommandListManager* commandManager, LinearAllocatorType type)
{
	mDevice = device;
	mCommandManager = commandManager;
	mAllocationType = type;
}

LinearAllocatorPageManager::~LinearAllocatorPageManager()
{
	for (auto it = mPagePool.begin(); it != mPagePool.end(); it++)
	{
		delete (*it);
	}
	mPagePool.clear();
}

LinearAllocationPage* LinearAllocatorPageManager::RequestPage()
{
	std::lock_guard<std::mutex> lock(mMutex);

	while (!mRetiredPages.empty() && mCommandManager->IsFenceComplete(mRetiredPages.front().first))
	{
		mAvailablePages.push(mRetiredPages.front().second);
		mRetiredPages.pop();
	}

	LinearAllocationPage* page = nullptr;

	if (!mAvailablePages.empty())
	{
		page = mAvailablePages.front();
		mAvailablePages.pop();
	}
	else
	{
		page = CreateNewPage();
		mPagePool.emplace_back(page);
	}

	return page;
}

LinearAllocationPage* LinearAllocatorPageManager::CreateNewPage(size_t size)
{
	D3D12_HEAP_PROPERTIES heapProps;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resDesc;
	resDesc.Alignment = 0;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	D3D12_RESOURCE_STATES usage;

	switch (mAllocationType)
	{
	case GPU_EXCLUSIVE:
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		resDesc.Width = size == 0 ? GPU_DEFAULT_PAGE_SIZE : size;
		resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		usage = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		break;

	case CPU_WRITABLE:
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		resDesc.Width = size == 0 ? CPU_DEFAULT_PAGE_SIZE : size;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		usage = D3D12_RESOURCE_STATE_GENERIC_READ;
		break;

	case CPU_READABLE:
		heapProps.Type = D3D12_HEAP_TYPE_READBACK;
		resDesc.Width = size == 0 ? GPU_DEFAULT_PAGE_SIZE : size;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		usage = D3D12_RESOURCE_STATE_COPY_DEST;
		break;

	default:
		break;
	}

	ID3D12Resource* buffer;
	mDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc, usage, nullptr, __uuidof(ID3D12Resource), (void**)&buffer);

	return new LinearAllocationPage(buffer, usage);
}

void LinearAllocatorPageManager::DiscardPages(uint64_t fenceValue, const std::vector<LinearAllocationPage*>& pages)
{
	std::lock_guard<std::mutex> lock(mMutex);

	for (auto it = pages.begin(); it != pages.end(); it++)
	{
		mRetiredPages.push(std::make_pair(fenceValue, *it));
	}
}

void LinearAllocatorPageManager::FreeLargePages(uint64_t fenceValue, const std::vector<LinearAllocationPage*>& pages)
{
	std::lock_guard<std::mutex> lock(mMutex);

	while (!mDeletionQueue.empty() && mCommandManager->IsFenceComplete(mDeletionQueue.front().first))
	{
		delete mDeletionQueue.front().second;
		mDeletionQueue.pop();
	}

	for (auto it = pages.begin(); it != pages.end(); it++)
	{
		(*it)->Unmap();
		mDeletionQueue.push(std::make_pair(fenceValue, *it));
	}
}

LinearAllocator::LinearAllocator(ID3D12Device* device, CommandListManager* commandManager, LinearAllocatorType type)
{
	mDevice = device;
	mCommandManager = commandManager;
	mAllocationType = type;
	mPageSize = (type == GPU_EXCLUSIVE ? GPU_DEFAULT_PAGE_SIZE : CPU_DEFAULT_PAGE_SIZE);
	mPageManager = new LinearAllocatorPageManager(mDevice, mCommandManager, mAllocationType);
}

LinearAllocator::~LinearAllocator()
{
	delete mPageManager;
}

DynAlloc LinearAllocator::Allocate(size_t size, size_t alignment)
{
	size_t alignmentMask = alignment - 1;
	size_t alignedSize = ((size + alignmentMask) & ~alignmentMask);

	if (alignedSize > mPageSize)
	{
		return AllocateLargePage(alignedSize);
	}

	mCurrentOffset = ((mCurrentOffset + alignmentMask) & ~alignmentMask);

	if (mCurrentOffset + alignedSize > mPageSize)
	{
		mRetiredPages.push_back(mCurrentPage);
		mCurrentPage = nullptr;
	}

	if (mCurrentPage == nullptr)
	{
		mCurrentPage = mPageManager->RequestPage();
		mCurrentOffset = 0;
	}

	DynAlloc ret(*mCurrentPage, mCurrentOffset, alignedSize);
	ret.mDataPtr = (uint8_t*)mCurrentPage->mCpuVirtualAddress + mCurrentOffset;
	ret.mGpuAddress = mCurrentPage->mGpuVirtualAddress + mCurrentOffset;

	mCurrentOffset += alignedSize;

	return ret;
}

void LinearAllocator::CleanupUsedPages(uint64_t fenceValue)
{
	mPageManager->DiscardPages(fenceValue, mRetiredPages);
	mRetiredPages.clear();

	mPageManager->FreeLargePages(fenceValue, mLargePageList);
	mLargePageList.clear();

	if (mCurrentPage == nullptr)
	{
		return;
	}

	mRetiredPages.push_back(mCurrentPage);
	mCurrentPage = nullptr;
	mCurrentOffset = 0;
}

DynAlloc LinearAllocator::AllocateLargePage(size_t size)
{
	LinearAllocationPage* page = mPageManager->CreateNewPage(size);
	mLargePageList.push_back(page);

	DynAlloc ret(*page, 0, size);
	ret.mDataPtr = (uint8_t*)page->mCpuVirtualAddress;
	ret.mGpuAddress = page->mGpuVirtualAddress;

	return ret;
}