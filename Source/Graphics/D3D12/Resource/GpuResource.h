#ifndef __GPU_RESOURCE__H__
#define __GPU_RESOURCE__H__

#include <d3d12.h>
#include <dxgi1_5.h>
#include "../Core/ResourceBarrier.h"
#include "../Core/CommandList.h"

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL 0ull
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN ~0ull

namespace Engine
{
	class GpuResource
	{
		friend class Context;
		friend class GraphicsContext;
		friend class ComputeContext;

	protected:
		ID3D12Resource* mResource;
		D3D12_GPU_VIRTUAL_ADDRESS mGpuVirtualAddress;
		D3D12_RESOURCE_STATES mUsageState;
		D3D12_RESOURCE_STATES mTransitionState;

	public:
		GpuResource()
		{
			mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
			mUsageState = D3D12_RESOURCE_STATE_COMMON;
			mTransitionState = (D3D12_RESOURCE_STATES)-1;
		}

		GpuResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES currentState)
		{
			mResource = resource;
			mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
			mUsageState = currentState;
			mTransitionState = (D3D12_RESOURCE_STATES)-1;
		}

		virtual ~GpuResource()
		{
			if (mResource != nullptr)
			{
				mResource->Release();
				mResource = nullptr;
			}
		}

		ID3D12Resource* Get() { return mResource; }
		const ID3D12Resource* Get() const { return mResource; }
		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return mGpuVirtualAddress; }
	};
}

#endif 