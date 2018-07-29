#ifndef __HEAP__H__
#define __HEAP__H__

#include "Core/Memory/Memory.h"
#include "DescriptorHandle.h"
#include <d3d12.h>
#include <vector>

namespace Engine
{
	class DescriptorHeap
	{
	private:
		ID3D12DescriptorHeap* mHeap;
		size_t mItemSize;
		size_t mItemsCount;
		bool mShaderVisible;
		D3D12_CPU_DESCRIPTOR_HANDLE mCpuStart;
		D3D12_GPU_DESCRIPTOR_HANDLE mGpuStart;
		std::vector<unsigned int> mFreeList;

	public:
		enum Type
		{
			RENDER_TARGET_VIEW = 0,
			DEPTH_STENCIL_VIEW,
			CBV_SRV_UAV,
			DESCRIPTOR_HEAP_TYPE_COUNT
		};

		static D3D12_DESCRIPTOR_HEAP_TYPE DescHeapType[];

		DescriptorHeap(ID3D12Device* device, Type type, unsigned int count, bool shaderVisible = true)
		{
			mItemSize = device->GetDescriptorHandleIncrementSize(DescHeapType[type]);
			mItemsCount = count;
			mShaderVisible = shaderVisible;
			if (type == RENDER_TARGET_VIEW || type == DEPTH_STENCIL_VIEW)
			{
				mShaderVisible = false;
			}

			D3D12_DESCRIPTOR_HEAP_DESC desc;
			if (mShaderVisible)
			{
				desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			}
			else
			{
				desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			}
			desc.NodeMask = 0;
			desc.NumDescriptors = count;
			desc.Type = DescHeapType[(int)type];

			device->CreateDescriptorHeap(&desc, __uuidof(ID3D12DescriptorHeap), (void**)&mHeap);

			for (unsigned int i = 0; i < count; i++)
			{
				mFreeList.push_back(i);
			}

			mCpuStart = mHeap->GetCPUDescriptorHandleForHeapStart();
			mGpuStart = mHeap->GetGPUDescriptorHandleForHeapStart();
		}

		~DescriptorHeap()
		{
			mHeap->Release();
		}

		DescriptorHandle Allocate()
		{
			unsigned int idx = mFreeList.back();
			mFreeList.pop_back();

			DescriptorHandle handle;
			size_t offset = idx * mItemSize;
			handle.mCpuHandle = mCpuStart;
			if (handle.mCpuHandle.ptr != 0)
			{
				handle.mCpuHandle.ptr += offset;
			}

			if (mShaderVisible)
			{
				handle.mGpuHandle = mGpuStart;
				if (handle.mGpuHandle.ptr != 0)
				{
					handle.mGpuHandle.ptr += offset;
				}
			}

			return handle;
		}

		void Free(DescriptorHandle& handle)
		{
			size_t offset = handle.mCpuHandle.ptr - mCpuStart.ptr;
			unsigned int idx = (unsigned int)(offset / mItemSize);
			mFreeList.push_back(idx);
		}

		DescriptorHandle Get(size_t i)
		{
			DescriptorHandle handle;
			size_t offset = i * mItemSize;
			handle.mCpuHandle = mCpuStart;
			if (handle.mCpuHandle.ptr != 0)
			{
				handle.mCpuHandle.ptr += offset;
			}

			if (mShaderVisible)
			{
				handle.mGpuHandle = mGpuStart;
				if (handle.mGpuHandle.ptr != 0)
				{
					handle.mGpuHandle.ptr += offset;
				}
			}

			return handle;
		}

		ID3D12DescriptorHeap* GetHeap()
		{
			return mHeap;
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::Heap")
	};
}

#endif