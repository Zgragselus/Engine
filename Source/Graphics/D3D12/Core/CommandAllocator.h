#ifndef __COMMAND_ALLOCATOR__H__
#define __COMMAND_ALLOCATOR__H__

#include <d3d12.h>
#include "Core/Memory/Memory.h"
#include <vector>
#include <queue>
#include <mutex>

namespace Engine
{
	class CommandList;

	class CommandAllocator
	{
		friend class CommandQueue;

	public:
		enum CommandListType
		{
			COMMAND_LIST_DIRECT = 0,
			COMMAND_LIST_COMPUTE = 1,
			COMMAND_LIST_BUNDLE = 2,
			COMMAND_LIST_COPY = 3
		};

	private:
		ID3D12CommandAllocator* mAllocator;
		CommandListType mType;
		static D3D12_COMMAND_LIST_TYPE CommandListTypeD3D[4];

	public:
		CommandAllocator(ID3D12Device* device, CommandListType cmdListType = COMMAND_LIST_DIRECT)
		{
			device->CreateCommandAllocator(CommandListTypeD3D[cmdListType], __uuidof(ID3D12CommandAllocator), (void**)&mAllocator);
			mType = cmdListType;
		}

		~CommandAllocator()
		{
			mAllocator->Release();
		}

		inline CommandListType GetType()
		{
			return mType;
		}

		inline ID3D12CommandAllocator* Get()
		{
			return mAllocator;
		}

		friend class CommandList;

		ALIGNED_NEW_DELETE("Engine::Graphics::CommanAllocator")
	};

	class CommandAllocatorPool
	{
	private:
		CommandAllocator::CommandListType mType;
		ID3D12Device* mDevice;
		std::vector<CommandAllocator*> mAllocatorPool;
		std::queue<std::pair<uint64_t, CommandAllocator*>> mReadyAllocators;
		std::mutex mAllocatorMutex;

	public:
		CommandAllocatorPool(ID3D12Device* device, CommandAllocator::CommandListType type);
		~CommandAllocatorPool();

		CommandAllocator* RequestAllocator(uint64_t completedFenceValue);
		void DiscardAllocator(uint64_t fenceValue, CommandAllocator* allocator);

		inline size_t Size() { return mAllocatorPool.size(); }

		ALIGNED_NEW_DELETE("Engine::Graphics::CommandAllocatorPool")
	};
}
	
#endif