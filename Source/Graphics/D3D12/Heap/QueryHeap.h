#ifndef __QUERY_HEAP__H__
#define __QUERY_HEAP__H__

#include "Core/Memory/Memory.h"
#include <d3d12.h>
#include <vector>

namespace Engine
{
	enum class QueryHeapType
	{
		OCCLUSION = 0,
		PIPELINE_STATS,
		STREAMOUT_STATS,
		TIMESTAMP,
		QUERY_TYPE_COUNT
	};

	class QueryHandle
	{
	public:
		QueryHeapType mType;
		unsigned int mHandle;

		ALIGNED_NEW_DELETE("Engine::Graphics::QueryHandle")
	};

	class QueryHeap
	{
	private:
		static D3D12_QUERY_HEAP_TYPE TypeD3D[4];
		ID3D12QueryHeap* mHeap;
		QueryHeapType mType;
		unsigned int mSize;
		unsigned int mNext;
		std::vector<unsigned int> mFreeList;

	public:
		QueryHeap(ID3D12Device* device, QueryHeapType type, unsigned int count)
		{
			mSize = count;
			mNext = 0;
			mType = type;

			D3D12_QUERY_HEAP_DESC desc;
			desc.Count = count;
			desc.Type = TypeD3D[(int)type];
			desc.NodeMask = 0;

			device->CreateQueryHeap(&desc, __uuidof(ID3D12QueryHeap), (void**)&mHeap);
		}

		~QueryHeap()
		{
			mHeap->Release();
		}

		QueryHandle Allocate()
		{
			QueryHandle handle;
			handle.mType = mType;

			if (mFreeList.size() > 0)
			{
				handle.mHandle = mFreeList.back();
				mFreeList.pop_back();
			}
			else
			{
				handle.mHandle = mNext;
				mNext++;
			}

			return handle;
		}

		void Free(const QueryHandle& handle)
		{
			mFreeList.push_back(handle.mHandle);
		}

		inline ID3D12QueryHeap* Get()
		{
			return mHeap;
		}

		inline D3D12_QUERY_HEAP_TYPE GetType(QueryHeapType type)
		{
			return TypeD3D[(int)mType];
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::QueryHeap")
	};
}

#endif