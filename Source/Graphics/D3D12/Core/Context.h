#ifndef __CONTEXT__H__
#define __CONTEXT__H__

#include "CommandAllocator.h"
#include "CommandList.h"
#include "../Resource/DepthBuffer.h"
#include "../Resource/ColorBuffer.h"
#include "RootSignature.h"
#include "../Pipeline/PipelineState.h"
#include "CommandQueue.h"
#include <queue>
#include <mutex>
#include "../Heap/DescriptorHeap.h"
#include "../Heap/QueryHeap.h"

namespace Engine
{
	class Context;
	class GpuResource;
	class GpuBuffer;
	class StructuredBuffer;
	class ByteAddressBuffer;

	struct DWParam
	{
		DWParam(float f) : Float(f) {}
		DWParam(int i) : Int(i) {}
		DWParam(unsigned int u) : Uint(u) {}

		void operator= (float f) { Float = f; }
		void operator= (unsigned int u) { Uint = u; }
		void operator= (int i) { Int = i; }
		
		union
		{
			float Float;
			unsigned int Uint;
			int Int;
		};
	};

	class ContextManager
	{
	private:
		CommandListManager* mOwnerManager;
		std::vector<Context*> mContextPool[4];
		std::queue<Context*> mAvailableContexts[4];
		std::mutex mContextAllocationMutex;

	public:
		ContextManager(CommandListManager* owner) { mOwnerManager = owner; }
		~ContextManager() { DestroyAllContexts(); }
		Context* AllocateContext(ID3D12Device* device, CommandAllocator::CommandListType type);
		void FreeContext(Context* context);
		void DestroyAllContexts();

		ALIGNED_NEW_DELETE("Engine::Graphics::ContextManager")
	};

	class Context
	{
		friend class ContextManager;

	protected:
		CommandListManager* mOwningManager;
		ContextManager* mOwningContext;
		CommandAllocator::CommandListType mType;
		CommandAllocator* mCmdAllocator;
		CommandList* mCmdList;
		RootSignature* mGraphicsRootSignature;
		PipelineState* mGraphicsPipelineState;
		RootSignature* mComputeRootSignature;
		PipelineState* mComputePipelineState;

		LinearAllocator* mGpuAllocator;
		LinearAllocator* mReadbackAllocator;
		LinearAllocator* mCpuAllocator;
		D3D12_RESOURCE_BARRIER mResourceBarriers[16];
		uint32_t mBarriersToFlush;

		ID3D12Device* mDevice;

		DescriptorHeap* mCurrentDescriptorHeaps[DescriptorHeap::DESCRIPTOR_HEAP_TYPE_COUNT];

		Context(ID3D12Device* device, CommandListManager* owner, ContextManager* ctxOwner, CommandAllocator::CommandListType type);
		virtual ~Context();

		inline size_t GetRequiredIntermediateSize(ID3D12Resource* resource,
			unsigned int firstSubresource,
			unsigned int numSubresources)
		{
			D3D12_RESOURCE_DESC desc = resource->GetDesc();
			size_t requiredSize = 0;

			ID3D12Device* device;
			resource->GetDevice(__uuidof(ID3D12Device), (void**)&device);
			device->GetCopyableFootprints(&desc, firstSubresource, numSubresources, 0, NULL, NULL, NULL, &requiredSize);
			device->Release();

			return requiredSize;
		}

		inline void MemcpySubresource( const D3D12_MEMCPY_DEST* dest,
			const D3D12_SUBRESOURCE_DATA* src,
			size_t rowSizeInBytes,
			unsigned int numRows,
			unsigned int numSlices)
		{
			for (unsigned int z = 0; z < numSlices; ++z)
			{
				unsigned char* destSlice = reinterpret_cast<unsigned char*>(dest->pData) + dest->SlicePitch * z;
				const unsigned char* srcSlice = reinterpret_cast<const unsigned char*>(src->pData) + src->SlicePitch * z;
				for (unsigned int y = 0; y < numRows; ++y)
				{
					memcpy(destSlice + dest->RowPitch * y,
						srcSlice + src->RowPitch * y,
						rowSizeInBytes);
				}
			}
		}

		inline size_t UpdateSubresources(CommandList* cmdList,
			ID3D12Resource* destination,
			ID3D12Resource* intermediate,
			size_t firstSubresource,
			size_t numSubresources,
			size_t requiredSize,
			const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* layouts,
			const unsigned int* numRows,
			const size_t* rowSizesInBytes,
			const D3D12_SUBRESOURCE_DATA* srcData)
		{
			// Validation
			D3D12_RESOURCE_DESC intermediateDesc = intermediate->GetDesc();
			D3D12_RESOURCE_DESC destinationDesc = destination->GetDesc();
			if (intermediateDesc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER ||
				intermediateDesc.Width < requiredSize + layouts[0].Offset ||
				requiredSize > (size_t)-1 ||
				(destinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER &&
				(firstSubresource != 0 || numSubresources != 1)))
			{
				return 0;
			}

			unsigned char* data;
			HRESULT hr = intermediate->Map(0, NULL, reinterpret_cast<void**>(&data));
			if (FAILED(hr))
			{
				return 0;
			}

			for (unsigned int i = 0; i < numSubresources; ++i)
			{
				if (rowSizesInBytes[i] > (size_t)-1)
				{
					return 0;
				}

				D3D12_MEMCPY_DEST destData = { data + layouts[i].Offset, layouts[i].Footprint.RowPitch, layouts[i].Footprint.RowPitch * numRows[i] };
				MemcpySubresource(&destData, &srcData[i], (size_t)rowSizesInBytes[i], numRows[i], layouts[i].Footprint.Depth);
			}
			intermediate->Unmap(0, NULL);

			if (destinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
			{
				D3D12_BOX box;
				box.left = unsigned int(layouts[0].Offset);
				box.top = 0;
				box.front = 0;
				box.right = unsigned int(layouts[0].Offset + layouts[0].Footprint.Width);
				box.bottom = 1;
				box.back = 1;
				cmdList->Get()->CopyBufferRegion(destination, 0, intermediate, layouts[0].Offset, layouts[0].Footprint.Width);
			}
			else
			{
				for (unsigned int i = 0; i < numSubresources; ++i)
				{
					D3D12_TEXTURE_COPY_LOCATION dst;
					dst.pResource = destination;
					dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
					dst.SubresourceIndex = i + (unsigned int)firstSubresource;

					D3D12_TEXTURE_COPY_LOCATION src;
					src.pResource = intermediate;
					src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
					src.PlacedFootprint = layouts[i];

					cmdList->Get()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
				}
			}

			return requiredSize;
		}

		inline size_t UpdateSubresources(CommandList* cmdList,
			ID3D12Resource* destination,
			ID3D12Resource* intermediate,
			size_t intermediateOffset,
			unsigned int firstSubresource,
			unsigned int numSubresources,
			D3D12_SUBRESOURCE_DATA* srcData)
		{
			size_t requiredSize = 0;
			size_t memToAlloc = static_cast<size_t>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(unsigned int) + sizeof(size_t)) * numSubresources;
			if (memToAlloc > SIZE_MAX)
			{
				return 0;
			}

			void* mem = HeapAlloc(GetProcessHeap(), 0, static_cast<size_t>(memToAlloc));
			if (mem == NULL)
			{
				return 0;
			}

			D3D12_PLACED_SUBRESOURCE_FOOTPRINT* layouts = reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(mem);
			size_t* rowSizesInBytes = reinterpret_cast<size_t*>(layouts + numSubresources);
			unsigned int* numRows = reinterpret_cast<unsigned int*>(rowSizesInBytes + numSubresources);

			D3D12_RESOURCE_DESC desc = destination->GetDesc();
			ID3D12Device* device;
			destination->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device));
			device->GetCopyableFootprints(&desc, firstSubresource, numSubresources, intermediateOffset, layouts, numRows, rowSizesInBytes, &requiredSize);
			device->Release();

			size_t result = UpdateSubresources(cmdList, destination, intermediate, firstSubresource, numSubresources, requiredSize, layouts, numRows, rowSizesInBytes, srcData);
			HeapFree(GetProcessHeap(), 0, mem);

			return result;
		}

		template <unsigned int MAX_SUBRESROUCES>
		inline size_t UpdateSubresources(
			CommandList* cmdList,
			ID3D12Resource* destination,
			ID3D12Resource* intermediate,
			size_t intermediateOffset,
			unsigned int firstSubresource,
			unsigned int numSubresources,
			D3D12_SUBRESOURCE_DATA* srcData)
		{
			size_t requiredSize = 0;
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT layouts[MAX_SUBRESROUCES];
			unsigned int numRows[MAX_SUBRESROUCES];
			size_t rowSizesInBytes[MAX_SUBRESROUCES];

			D3D12_RESOURCE_DESC desc = destination->GetDesc();
			ID3D12Device* device;
			destination->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device));
			device->GetCopyableFootprints(&desc, firstSubresource, numSubresources, intermediateOffset, layouts, numRows, rowSizesInBytes, &requiredSize);
			device->Release();

			return UpdateSubresources(cmdList, destination, intermediate, firstSubresource, numSubresources, requiredSize, layouts, numRows, rowSizesInBytes, srcData);
		}

	public:
		void Begin(PipelineState* state = NULL);
		uint64_t Flush();
		uint64_t Finish();
		void BindDescriptorHeaps();

		CommandList* GetCommandList()
		{
			return mCmdList;
		}

		DynAlloc ReserveUploadMemory(size_t size)
		{
			return mCpuAllocator->Allocate(size);
		}

		DynAlloc ReserveReadbackMemory(size_t size)
		{
			return mReadbackAllocator->Allocate(size);
		}

		void CopyBuffer(GpuResource* dest, GpuResource* src);
		void CopyBufferRegion(GpuResource* dest, size_t destOffset, GpuResource* src, size_t srcOffset, size_t numBytes);
		void CopySubresource(GpuResource* dest, unsigned int destSubIndex, GpuResource* src, unsigned int srcSubIndex);
		void CopyCounter(GpuResource* dest, size_t destOffset, StructuredBuffer* src, size_t bytes = 4);
		void ResetCounter(StructuredBuffer* buffer, uint32_t value = 0, size_t bytes = 4);
		void CopyCounter(GpuResource* dest, size_t destOffset, ByteAddressBuffer* src, size_t bytes = 4);
		void ResetCounter(ByteAddressBuffer* buffer, uint32_t value = 0, size_t bytes = 4);

		void InitializeBuffer(GpuResource* buffer, const void* data, size_t size, size_t offset = 0);
		void InitializeComputeBuffer(GpuResource* buffer, const void* data, size_t size, size_t offset = 0);
		void InitializeTexture(GpuResource* texture, unsigned int numSubresources, D3D12_SUBRESOURCE_DATA data[]);
		void ReadbackBuffer(GpuResource* buffer, void* data, size_t size, size_t offset = 0);
		void ReadbackTexture2D(GpuResource* readbackBuffer, PixelBuffer* srcBuffer, uint32_t miplevel = 0);

		void WriteBuffer(GpuResource* dest, size_t destOffset, const void* data, size_t numBytes);
		void FillBuffer(GpuResource* dest, size_t destOffset, DWParam value, size_t numBytes);

		void TransitionResource(GpuResource* resource, D3D12_RESOURCE_STATES newState, bool flushImmediate = false);
		void BeginResourceTransition(GpuResource* resource, D3D12_RESOURCE_STATES newState, bool flushImmediate = false);
		void InsertUAVBarrier(GpuResource* resource, bool flushImmediate = false);
		void InsertAliasBarrier(GpuResource* before, GpuResource* after, bool flushImmediate = false);
		void FlushResourceBarriers();

		inline void BeginQuery(QueryHeap* heap, D3D12_QUERY_TYPE type, QueryHandle& handle);
		inline void EndQuery(QueryHeap* heap, D3D12_QUERY_TYPE type, QueryHandle& handle);
		inline void ResolveQueryData(QueryHeap* heap, D3D12_QUERY_TYPE type, QueryHandle& firstHandle, unsigned int handleCount, GpuResource* dest, size_t destOffset);

		uint64_t GetTimestampFrequency();

		void SetDescriptorHeap(DescriptorHeap::Type type, DescriptorHeap* heap)
		{
			if (mCurrentDescriptorHeaps[type] != heap)
			{
				mCurrentDescriptorHeaps[type] = heap;
				BindDescriptorHeaps();
			}
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::Context")
	};

	class GraphicsContext : public Context
	{
	protected:
		GraphicsContext(ID3D12Device* device, CommandListManager* owner, ContextManager* ctxOwner);
		virtual ~GraphicsContext();

	public:
		void SetViewport(float left, float top, float width, float height, float minZ = 0.0f, float maxZ = 1.0f)
		{
			D3D12_VIEWPORT viewport = D3D12_VIEWPORT();
			viewport.TopLeftX = left;
			viewport.TopLeftY = top;
			viewport.MinDepth = minZ;
			viewport.MaxDepth = maxZ;
			viewport.Width = width;
			viewport.Height = height;

			mCmdList->Get()->RSSetViewports(1, &viewport);
		}

		void SetScissorRect(float left, float top, float width, float height)
		{
			D3D12_RECT rect = D3D12_RECT();
			rect.left = (LONG)left;
			rect.right = (LONG)left + (LONG)width;
			rect.top = (LONG)top;
			rect.bottom = (LONG)top + (LONG)height;

			mCmdList->Get()->RSSetScissorRects(1, &rect);
		}

		void SetPrimitiveTopology(Graphics::Topology topology)
		{
			mCmdList->Get()->IASetPrimitiveTopology(Graphics::GetTopology(topology));
		}

		void ClearColor(ColorBuffer* rtv, float red, float green, float blue, float alpha)
		{
			float color[4];
			color[0] = red;
			color[1] = green;
			color[2] = blue;
			color[3] = alpha;

			mCmdList->Get()->ClearRenderTargetView(rtv->GetRTV().mCpuHandle, color, 0, NULL);
		}

		void ClearColor(const DescriptorHandle& rtv, float red, float green, float blue, float alpha)
		{
			float color[4];
			color[0] = red;
			color[1] = green;
			color[2] = blue;
			color[3] = alpha;

			mCmdList->Get()->ClearRenderTargetView(rtv.mCpuHandle, color, 0, NULL);
		}

		void ClearDepth(DepthBuffer* dsv, float depth, int stencil)
		{
			mCmdList->Get()->ClearDepthStencilView(dsv->GetDSV().mCpuHandle, D3D12_CLEAR_FLAG_DEPTH, depth, stencil, 0, NULL);
		}

		void ClearDepth(const DescriptorHandle& dsv, float depth, int stencil)
		{
			mCmdList->Get()->ClearDepthStencilView(dsv.mCpuHandle, D3D12_CLEAR_FLAG_DEPTH, depth, stencil, 0, NULL);
		}

		void SetRootSignature(RootSignature* sig)
		{
			mGraphicsRootSignature = sig;
			mCmdList->Get()->SetGraphicsRootSignature(sig->Get());
		}

		void SetPipelineState(PipelineState* pso)
		{
			mGraphicsPipelineState = pso;
			mCmdList->Get()->SetPipelineState(pso->Get());
		}

		inline void SetConstants(unsigned int rootIndex, unsigned int numConstants, const void* data);
		inline void SetConstants(unsigned int rootIndex, DWParam x);
		inline void SetConstants(unsigned int rootIndex, DWParam x, DWParam y);
		inline void SetConstants(unsigned int rootIndex, DWParam x, DWParam y, DWParam z);
		inline void SetConstants(unsigned int rootIndex, DWParam x, DWParam y, DWParam z, DWParam w);
		inline void SetConstantMatrix(unsigned int rootIndex, float* ptr);
		inline void SetConstantBuffer(unsigned int rootIndex, D3D12_GPU_VIRTUAL_ADDRESS CBV);
		void SetBufferSRV(unsigned int rootIndex, const GpuBuffer& SRV, uint64_t offset = 0);
		void SetBufferUAV(unsigned int rootIndex, const GpuBuffer& UAV, uint64_t offset = 0);
		inline void SetDescriptorTable(UINT rootIndex, const DescriptorHandle& firstHandle);

		void SetRenderTargets(unsigned int numRTs, ColorBuffer** rtvs)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE tmp[8] = { 0 };
			for (unsigned int i = 0; i < numRTs; i++)
			{
				tmp[i] = rtvs[i]->GetRTV().mCpuHandle;
			}

			mCmdList->Get()->OMSetRenderTargets(numRTs, tmp, false, nullptr);
		}

		void SetRenderTargets(unsigned int numRTs, ColorBuffer** rtvs, DepthBuffer* dsv)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE tmp[8] = { 0 };
			for (unsigned int i = 0; i < numRTs; i++)
			{
				tmp[i] = rtvs[i]->GetRTV().mCpuHandle;
			}

			mCmdList->Get()->OMSetRenderTargets(numRTs, tmp, false, &(dsv->GetDSV().mCpuHandle));
		}

		void SetRenderTargets(ColorBuffer* rtv, DepthBuffer* dsv)
		{
			mCmdList->Get()->OMSetRenderTargets(1, &(rtv->GetRTV().mCpuHandle), false, &(dsv->GetDSV().mCpuHandle));
		}

		void SetRenderTargets(const DescriptorHandle& rtv, const DescriptorHandle& dsv)
		{
			mCmdList->Get()->OMSetRenderTargets(1, &(rtv.mCpuHandle), false, &(dsv.mCpuHandle));
		}

		void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& view)
		{
			mCmdList->Get()->IASetIndexBuffer(&view);
		}

		void SetVertexBuffer(unsigned int slot, const D3D12_VERTEX_BUFFER_VIEW& view)
		{
			mCmdList->Get()->IASetVertexBuffers(slot, 1, &view);
		}

		void SetVertexBuffers(unsigned int startSlot, unsigned int count, const D3D12_VERTEX_BUFFER_VIEW views[])
		{
			mCmdList->Get()->IASetVertexBuffers(startSlot, count, views);
		}

		void Draw(unsigned int vertexCount, unsigned int vertexOffset)
		{
			DrawInstanced(vertexCount, 1, vertexOffset, 0);
		}

		void DrawIndexed(unsigned int indexCount, unsigned int startIndex, int baseVertex)
		{
			DrawIndexedInstanced(indexCount, 1, startIndex, baseVertex, 0);
		}

		void DrawInstanced(unsigned int indexCount, unsigned int instanceCount, unsigned int startIndex, int startInstance)
		{
			FlushResourceBarriers();
			mCmdList->Get()->DrawInstanced(indexCount, instanceCount, startIndex, startInstance);
		}

		void DrawIndexedInstanced(unsigned int indexCountPerInstance, unsigned int instanceCount, unsigned int startIndex, int baseVertex, unsigned int startInstance)
		{
			FlushResourceBarriers();
			mCmdList->Get()->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndex, baseVertex, startInstance);
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::GraphicsContext")
	};

	class ComputeContext : public Context
	{
	public:
		inline void SetRootSignature(RootSignature* sig)
		{
			mComputeRootSignature = sig;
			mCmdList->Get()->SetComputeRootSignature(sig->Get());
		}

		inline void SetPipelineState(PipelineState* pso)
		{
			mComputePipelineState = pso;
			mCmdList->Get()->SetPipelineState(pso->Get());
		}

		inline void SetConstants(unsigned int rootIndex, unsigned int numConstants, const void* data);
		inline void SetConstants(unsigned int rootIndex, DWParam x);
		inline void SetConstants(unsigned int rootIndex, DWParam x, DWParam y);
		inline void SetConstants(unsigned int rootIndex, DWParam x, DWParam y, DWParam z);
		inline void SetConstants(unsigned int rootIndex, DWParam x, DWParam y, DWParam z, DWParam w);
		inline void SetConstantMatrix(unsigned int rootIndex, float* ptr);
		inline void SetConstantBuffer(unsigned int rootIndex, D3D12_GPU_VIRTUAL_ADDRESS CBV);
		inline void SetDescriptorTable(UINT rootIndex, const DescriptorHandle& firstHandle);

		void Dispatch(unsigned int threadCountX, unsigned int threadCountY, unsigned int threadCountZ)
		{
			FlushResourceBarriers();
			mCmdList->Get()->Dispatch(threadCountX, threadCountY, threadCountZ);
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::ComputeContext")
	};

	inline void Context::BeginQuery(QueryHeap* heap, D3D12_QUERY_TYPE type, QueryHandle& handle)
	{
		mCmdList->Get()->BeginQuery(heap->Get(), type, handle.mHandle);
	}

	inline void Context::EndQuery(QueryHeap* heap, D3D12_QUERY_TYPE type, QueryHandle& handle)
	{
		mCmdList->Get()->EndQuery(heap->Get(), type, handle.mHandle);
	}
	
	inline void Context::ResolveQueryData(QueryHeap* heap, D3D12_QUERY_TYPE type, QueryHandle& firstHandle, unsigned int handleCount, GpuResource* dest, size_t destOffset)
	{
		mCmdList->Get()->ResolveQueryData(heap->Get(), type, firstHandle.mHandle, handleCount, dest->Get(), destOffset);
	}

	inline void GraphicsContext::SetConstants(unsigned int rootIndex, unsigned int numConstants, const void* data)
	{
		mCmdList->Get()->SetGraphicsRoot32BitConstants(rootIndex, numConstants, data, 0);
	}

	inline void GraphicsContext::SetConstants(unsigned int rootIndex, DWParam x)
	{
		mCmdList->Get()->SetGraphicsRoot32BitConstant(rootIndex, x.Uint, 0);
	}

	inline void GraphicsContext::SetConstants(unsigned int rootIndex, DWParam x, DWParam y)
	{
		mCmdList->Get()->SetGraphicsRoot32BitConstant(rootIndex, x.Uint, 0);
		mCmdList->Get()->SetGraphicsRoot32BitConstant(rootIndex, y.Uint, 1);
	}

	inline void GraphicsContext::SetConstants(unsigned int rootIndex, DWParam x, DWParam y, DWParam z)
	{
		mCmdList->Get()->SetGraphicsRoot32BitConstant(rootIndex, x.Uint, 0);
		mCmdList->Get()->SetGraphicsRoot32BitConstant(rootIndex, y.Uint, 1);
		mCmdList->Get()->SetGraphicsRoot32BitConstant(rootIndex, z.Uint, 2);
	}

	inline void GraphicsContext::SetConstants(unsigned int rootIndex, DWParam x, DWParam y, DWParam z, DWParam w)
	{
		mCmdList->Get()->SetGraphicsRoot32BitConstant(rootIndex, x.Uint, 0);
		mCmdList->Get()->SetGraphicsRoot32BitConstant(rootIndex, y.Uint, 1);
		mCmdList->Get()->SetGraphicsRoot32BitConstant(rootIndex, z.Uint, 2);
		mCmdList->Get()->SetGraphicsRoot32BitConstant(rootIndex, w.Uint, 3);
	}

	inline void GraphicsContext::SetConstantMatrix(unsigned int rootIndex, float* ptr)
	{
		for (int i = 0; i < 16; i++)
		{
			mCmdList->Get()->SetGraphicsRoot32BitConstant(rootIndex, *(UINT*)(ptr + i), i);
		}
	}

	inline void GraphicsContext::SetConstantBuffer(unsigned int rootIndex, D3D12_GPU_VIRTUAL_ADDRESS CBV)
	{
		mCmdList->Get()->SetGraphicsRootConstantBufferView(rootIndex, CBV);
	}

	inline void GraphicsContext::SetDescriptorTable(UINT rootIndex, const DescriptorHandle& firstHandle)
	{
		mCmdList->Get()->SetGraphicsRootDescriptorTable(rootIndex, firstHandle.mGpuHandle);
	}

	inline void ComputeContext::SetConstants(unsigned int rootIndex, unsigned int numConstants, const void* data)
	{
		mCmdList->Get()->SetComputeRoot32BitConstants(rootIndex, numConstants, data, 0);
	}

	inline void ComputeContext::SetConstants(unsigned int rootIndex, DWParam x)
	{
		mCmdList->Get()->SetComputeRoot32BitConstant(rootIndex, x.Uint, 0);
	}

	inline void ComputeContext::SetConstants(unsigned int rootIndex, DWParam x, DWParam y)
	{
		mCmdList->Get()->SetComputeRoot32BitConstant(rootIndex, x.Uint, 0);
		mCmdList->Get()->SetComputeRoot32BitConstant(rootIndex, y.Uint, 1);
	}

	inline void ComputeContext::SetConstants(unsigned int rootIndex, DWParam x, DWParam y, DWParam z)
	{
		mCmdList->Get()->SetComputeRoot32BitConstant(rootIndex, x.Uint, 0);
		mCmdList->Get()->SetComputeRoot32BitConstant(rootIndex, y.Uint, 1);
		mCmdList->Get()->SetComputeRoot32BitConstant(rootIndex, z.Uint, 2);
	}

	inline void ComputeContext::SetConstants(unsigned int rootIndex, DWParam x, DWParam y, DWParam z, DWParam w)
	{
		mCmdList->Get()->SetComputeRoot32BitConstant(rootIndex, x.Uint, 0);
		mCmdList->Get()->SetComputeRoot32BitConstant(rootIndex, y.Uint, 1);
		mCmdList->Get()->SetComputeRoot32BitConstant(rootIndex, z.Uint, 2);
		mCmdList->Get()->SetComputeRoot32BitConstant(rootIndex, w.Uint, 3);
	}

	inline void ComputeContext::SetConstantMatrix(unsigned int rootIndex, float* ptr)
	{
		for (int i = 0; i < 16; i++)
		{
			mCmdList->Get()->SetComputeRoot32BitConstant(rootIndex, *(UINT*)(ptr + i), i);
		}
	}

	inline void ComputeContext::SetConstantBuffer(unsigned int rootIndex, D3D12_GPU_VIRTUAL_ADDRESS CBV)
	{
		mCmdList->Get()->SetComputeRootConstantBufferView(rootIndex, CBV);
	}

	inline void ComputeContext::SetDescriptorTable(UINT rootIndex, const DescriptorHandle& firstHandle)
	{
		mCmdList->Get()->SetComputeRootDescriptorTable(rootIndex, firstHandle.mGpuHandle);
	}
}

#endif