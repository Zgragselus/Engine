#ifndef __LINEAR_ALLOCATOR__H__
#define __LINEAR_ALLOCATOR__H__

#include "GpuResource.h"
#include <vector>
#include <mutex>
#include <queue>

namespace Engine
{
	class CommandListManager;

	struct DynAlloc
	{
		DynAlloc(GpuResource& resource, size_t offset, size_t size) :
			mBuffer(resource), mOffset(offset), mSize(size)
		{

		}

		GpuResource& mBuffer;						// D3D12 Buffer associated with memory
		size_t mOffset;								// Offset from start of the buffer resource
		size_t mSize;								// Reserved size for this allocation
		void* mDataPtr;								// CPU writeable address
		D3D12_GPU_VIRTUAL_ADDRESS mGpuAddress;		// GPU visible address
	};

	class LinearAllocationPage : public GpuResource
	{
	public:
		void* mCpuVirtualAddress;
		D3D12_GPU_VIRTUAL_ADDRESS mGpuVirtualAddress;

		LinearAllocationPage(ID3D12Resource* resource, D3D12_RESOURCE_STATES currentState) : GpuResource(resource, currentState)
		{
			mGpuVirtualAddress = mResource->GetGPUVirtualAddress();
			mResource->Map(0, nullptr, &mCpuVirtualAddress);
		}

		~LinearAllocationPage()
		{
			Unmap();
		}

		void Map()
		{
			if (mCpuVirtualAddress == nullptr)
			{
				mResource->Map(0, nullptr, &mCpuVirtualAddress);
			}
		}

		void Unmap()
		{
			if (mCpuVirtualAddress != nullptr)
			{
				mResource->Unmap(0, nullptr);
				mCpuVirtualAddress = nullptr;
			}
		}
	};

	enum LinearAllocatorType
	{
		INVALID_ALLOCATOR = -1,
		GPU_EXCLUSIVE = 0,			// Default (via UAV)
		CPU_WRITABLE = 1,			// Upload write-combined (CPU writable)
		CPU_READABLE = 2,			// Readback 
		NUM_ALLOCATOR_TYPES	
	};

	enum LinearAllocatorPageSize
	{
		GPU_DEFAULT_PAGE_SIZE = 0x10000,	// 64K
		CPU_DEFAULT_PAGE_SIZE = 0x200000	// 2MB
	};

	class LinearAllocatorPageManager
	{
	private:
		CommandListManager* mCommandManager;
		ID3D12Device* mDevice;
		LinearAllocatorType mAllocationType;
		std::vector<LinearAllocationPage*> mPagePool;
		std::queue<std::pair<uint64_t, LinearAllocationPage*>> mRetiredPages;
		std::queue<std::pair<uint64_t, LinearAllocationPage*>> mDeletionQueue;
		std::queue<LinearAllocationPage*> mAvailablePages;
		std::mutex mMutex;

	public:
		LinearAllocatorPageManager(ID3D12Device* device, CommandListManager* commandManager, LinearAllocatorType type);
		~LinearAllocatorPageManager();

		LinearAllocationPage* RequestPage();
		LinearAllocationPage* CreateNewPage(size_t size = 0);
		void DiscardPages(uint64_t fenceValue, const std::vector<LinearAllocationPage*>& pages);
		void FreeLargePages(uint64_t fenceValue, const std::vector<LinearAllocationPage*>& pages);
	};

	class LinearAllocator
	{
	private:
		CommandListManager* mCommandManager;
		ID3D12Device* mDevice;
		DynAlloc AllocateLargePage(size_t size);
		LinearAllocatorPageManager* mPageManager;
		LinearAllocatorType mAllocationType;
		size_t mPageSize;
		size_t mCurrentOffset;
		LinearAllocationPage* mCurrentPage;
		std::vector<LinearAllocationPage*> mRetiredPages;
		std::vector<LinearAllocationPage*> mLargePageList;

	public:
		LinearAllocator(ID3D12Device* device, CommandListManager* commandManager, LinearAllocatorType type);
		~LinearAllocator();
		DynAlloc Allocate(size_t size, size_t alignment = 256);
		void CleanupUsedPages(uint64_t fenceValue);

	};
}

#endif