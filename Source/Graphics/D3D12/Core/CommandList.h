#ifndef __COMMAND_LIST__H__
#define __COMMAND_LIST__H__

#include <d3d12.h>
#include "Core/Memory/Memory.h"
#include "CommandAllocator.h"

namespace Engine
{
	class CommandList
	{
	private:
		ID3D12GraphicsCommandList* mList;

	public:
		CommandList(ID3D12Device* device, CommandAllocator* allocator, CommandAllocator::CommandListType type = CommandAllocator::COMMAND_LIST_DIRECT)
		{
			device->CreateCommandList(0, CommandAllocator::CommandListTypeD3D[type], allocator->Get(), NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&mList);
			mList->Close();
		}

		~CommandList()
		{
			mList->Release();
		}

		inline ID3D12GraphicsCommandList* Get()
		{
			return mList;
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::CommandList")
	};
}

#endif