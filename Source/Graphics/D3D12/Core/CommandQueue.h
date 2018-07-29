///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CommandQueue.h
//
// CommanQueue is a structure that executes GPU-based commands put into CommandList
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __COMMAND_QUEUE__RENDERER__H__
#define __COMMAND_QUEUE__RENDERER__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Memory/Memory.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include "CommandAllocator.h"
#include "CommandList.h"
#include "../Resource/LinearAllocator.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// DirectX 12 renderer class
	/// </summary>
	class CommandQueue
	{
		friend class CommandListManager;
		friend class Context;
		friend class ContextManager;

	private:
		ID3D12CommandQueue* mQueue;
		
		CommandAllocator::CommandListType mType;

		CommandListManager* mManager;

		CommandAllocatorPool* mAllocatorPool;
		std::mutex mFenceMutex;
		std::mutex mEventMutex;

		ID3D12Fence* mFence;
		uint64_t mNextFenceValue;
		uint64_t mLastCompletedFenceValue;
		HANDLE mFenceEventHandle;

		uint64_t ExecuteCommandList(CommandList* list);
		CommandAllocator* RequestAllocator();
		void DiscardAllocator(uint64_t fenceValueForReset, CommandAllocator* allocator);

	public:
		CommandQueue(ID3D12Device* device, CommandListManager* manager, CommandAllocator::CommandListType type);
		~CommandQueue();

		inline bool IsReady()
		{
			return mQueue != nullptr;
		}

		uint64_t IncrementFence();
		bool IsFenceCompleted(uint64_t fenceValue);
		void StallForFence(uint64_t fenceValue);
		void StallForProducer(CommandQueue& producer);
		void WaitForFence(uint64_t fenceValue);
		void WaitForIdle()
		{
			WaitForFence(mNextFenceValue);
		}

		ID3D12CommandQueue* GetCommandQueue()
		{
			return mQueue;
		}

		uint64_t GetNextFenceValue()
		{
			return mNextFenceValue;
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::CommandQueue")
	};

	class CommandListManager
	{
	private:
		ID3D12Device* mDevice;
		CommandQueue* mGraphicsQueue;
		CommandQueue* mComputeQueue;
		CommandQueue* mCopyQueue;

	public:
		CommandListManager(ID3D12Device* device);
		~CommandListManager();

		CommandQueue* GetGraphicsQueue() { return mGraphicsQueue; }
		CommandQueue* GetComputeQueue() { return mComputeQueue; }
		CommandQueue* GetCopyQueue() { return mCopyQueue; }
		CommandQueue* GetQueue(CommandAllocator::CommandListType type)
		{
			switch (type)
			{
			case CommandAllocator::COMMAND_LIST_COMPUTE:
				return mComputeQueue;
				break;

			case CommandAllocator::COMMAND_LIST_COPY:
				return mCopyQueue;
				break;

			default:
				return mGraphicsQueue;
				break;
			}
		}

		void CreateNewCommandList(CommandAllocator::CommandListType type, CommandList** list, CommandAllocator** allocator);

		bool IsFenceComplete(uint64_t fenceValue)
		{
			return GetQueue(CommandAllocator::CommandListType(fenceValue >> 56))->IsFenceCompleted(fenceValue);

		}

		void WaitForFence(uint64_t fenceValue);

		void IdleGPU()
		{
			mGraphicsQueue->WaitForIdle();
			mComputeQueue->WaitForIdle();
			mCopyQueue->WaitForIdle();
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::CommandListManager")
	};
}

#endif