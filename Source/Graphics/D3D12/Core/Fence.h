#ifndef __FENCE__H__D3D__
#define __FENCE__H__D3D__

#include <d3d12.h>

namespace Engine
{
	class Fence
	{
	private:
		HANDLE mFenceEvent = INVALID_HANDLE_VALUE;

	public:
		ID3D12Fence* mFence = NULL;

		Fence(ID3D12Device* device, size_t initValue = 0)
		{
			device->CreateFence(initValue, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&mFence);
			mFenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		}

		~Fence()
		{
			mFence->Release();
		}

		void Signal(ID3D12CommandQueue* queue, size_t fenceValue)
		{
			queue->Signal(mFence, fenceValue);
		}

		void Wait(size_t fenceValue)
		{
			if (mFence->GetCompletedValue() < fenceValue)
			{
				mFence->SetEventOnCompletion(fenceValue, mFenceEvent);
				WaitForSingleObject(mFenceEvent, INFINITE);
			}
		}

		bool Signaled(size_t fenceValue)
		{
			return mFence->GetCompletedValue() >= fenceValue;
		}

		void Clear(size_t fenceValue)
		{
			mFence->Signal(fenceValue);
		}
	};
}

#endif
