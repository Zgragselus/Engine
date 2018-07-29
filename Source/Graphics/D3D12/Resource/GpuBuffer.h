#ifndef __GPU_BUFFER__H__
#define __GPU_BUFFER__H__

#include "GpuResource.h"
#include "../D3DRenderer.h"

namespace Engine
{
	class GpuBuffer : public GpuResource
	{
	protected:
		D3DRenderer* mRenderer;

		DescriptorHandle mUAV;
		DescriptorHandle mSRV;

		size_t mBufferSize;
		size_t mElemCount;
		size_t mElemSize;
		D3D12_RESOURCE_FLAGS mFlags;

		char* mBuffer;

		virtual void CreateDerivedViews() = 0;

	public:
		GpuBuffer() : GpuResource() 
		{
			mFlags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			mUAV.mCpuHandle.ptr = NULL;
			mSRV.mCpuHandle.ptr = NULL;
			mBuffer = nullptr;
		}

		virtual ~GpuBuffer()
		{
			if (mBuffer)
			{
				delete[] mBuffer;
			}

			if (mSRV.IsValid())
			{
				mRenderer->Heap()->Free(mSRV);
			}

			if (mUAV.IsValid())
			{
				mRenderer->Heap()->Free(mUAV);
			}
		}

		void Init(D3DRenderer* renderer, size_t numElements, size_t elementSize, const void* ptr)
		{
			mRenderer = renderer;

			mElemCount = numElements;
			mElemSize = elementSize;
			mBufferSize = mElemCount * mElemSize;

			D3D12_RESOURCE_DESC desc;
			desc.Alignment = 0;
			desc.DepthOrArraySize = 1;
			desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			desc.Flags = mFlags;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.Height = 1;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			desc.MipLevels = 1;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Width = mBufferSize;

			mUsageState = D3D12_RESOURCE_STATE_COMMON;

			D3D12_HEAP_PROPERTIES props;
			props.Type = D3D12_HEAP_TYPE_DEFAULT;
			props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			props.CreationNodeMask = 1;
			props.VisibleNodeMask = 1;

			HRESULT hr = renderer->GetDevice()->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, mUsageState, NULL, __uuidof(ID3D12Resource), (void**)&mResource);
			if (FAILED(hr))
			{
				HRESULT hr2 = renderer->GetDevice()->GetDeviceRemovedReason();
				printf("Error\n");
				return;
			}

			mGpuVirtualAddress = mResource->GetGPUVirtualAddress();

			if (ptr)
			{
				renderer->GetGraphicsContext()->InitializeBuffer(this, ptr, numElements * elementSize);
				
				if (mBuffer)
				{
					delete[] mBuffer;
				}
				mBuffer = new char[numElements * elementSize];
				memcpy(mBuffer, ptr, numElements * elementSize);
			}

			CreateDerivedViews();
		}

		const DescriptorHandle& GetUAV(void) const { return mUAV; }
		const DescriptorHandle& GetSRV(void) const { return mSRV; }

		inline D3D12_VERTEX_BUFFER_VIEW VertexBufferView(size_t offset, unsigned int size, unsigned int stride)
		{
			D3D12_VERTEX_BUFFER_VIEW view;
			view.BufferLocation = mGpuVirtualAddress + offset;
			view.SizeInBytes = size;
			view.StrideInBytes = stride;
			return view;
		}

		inline D3D12_INDEX_BUFFER_VIEW IndexBufferView(size_t offset, unsigned int size)
		{
			D3D12_INDEX_BUFFER_VIEW view;
			view.BufferLocation = mGpuVirtualAddress + offset;
			view.SizeInBytes = size;
			view.Format = DXGI_FORMAT_R32_UINT;
			return view;
		}

		inline D3D12_GPU_VIRTUAL_ADDRESS RootConstantBufferView()
		{
			return mGpuVirtualAddress;
		}

		inline DescriptorHandle CreateConstantBufferView(size_t offset, unsigned int size)
		{
			size_t alignmentMask = size - 1;
			size_t alignedSize = ((size + alignmentMask) & ~alignmentMask);

			D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
			desc.SizeInBytes = (unsigned int)alignedSize;
			desc.BufferLocation = mGpuVirtualAddress + offset;

			DescriptorHandle cBufferHandle = mRenderer->Heap()->Allocate();
			mRenderer->GetDevice()->CreateConstantBufferView(&desc, cBufferHandle.mCpuHandle);
			return cBufferHandle;
		}

		inline size_t Size()
		{
			return mBufferSize;
		}

		inline size_t NumElements()
		{
			return mElemCount;
		}

		inline size_t ElementSize()
		{
			return mElemSize;
		}

		inline char* Data()
		{
			return mBuffer;
		}
	};

	class ByteAddressBuffer : public GpuBuffer
	{
	public:
		virtual void CreateDerivedViews()
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Buffer.NumElements = (unsigned int)mBufferSize / 4;
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

			if (!mSRV.IsValid())
			{
				mSRV = mRenderer->Heap()->Allocate();
			}
			mRenderer->GetDevice()->CreateShaderResourceView(mResource, &srvDesc, mSRV.mCpuHandle);
			
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			uavDesc.Buffer.NumElements = (unsigned int)mBufferSize / 4;
			uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

			if (!mUAV.IsValid())
			{
				mUAV = mRenderer->Heap()->Allocate();
			}
			mRenderer->GetDevice()->CreateUnorderedAccessView(mResource, nullptr, &uavDesc, mUAV.mCpuHandle);
		}
	};

	class StructuredBuffer : public GpuBuffer
	{
	private:
		ByteAddressBuffer mCounterBuffer;

	public:
		virtual void CreateDerivedViews()
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Buffer.NumElements = (unsigned int)mElemCount;
			srvDesc.Buffer.StructureByteStride = (unsigned int)mElemSize;
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

			if (!mSRV.IsValid())
			{
				mSRV = mRenderer->Heap()->Allocate();
			}
			mRenderer->GetDevice()->CreateShaderResourceView(mResource, &srvDesc, mSRV.mCpuHandle);

			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			uavDesc.Buffer.NumElements = (unsigned int)mElemCount;
			uavDesc.Buffer.StructureByteStride = (unsigned int)mElemSize;
			uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

			mCounterBuffer.Init(mRenderer, 1, 4, NULL);

			if (!mUAV.IsValid())
			{
				mUAV = mRenderer->Heap()->Allocate();
			}
			mRenderer->GetDevice()->CreateUnorderedAccessView(mResource, mCounterBuffer.Get(), &uavDesc, mUAV.mCpuHandle);
		}

		ByteAddressBuffer& GetCounterBuffer()
		{
			return mCounterBuffer;
		}

		const DescriptorHandle& GetCounterSRV()
		{
			return mCounterBuffer.GetSRV();
		}

		const DescriptorHandle& GetCounterUAV()
		{
			return mCounterBuffer.GetUAV();
		}
	};

	class TypedBuffer : public GpuBuffer
	{
	private:
		Graphics::Format mFormat;

	public:
		TypedBuffer(Graphics::Format format)
		{
			mFormat = format;
		}

		virtual void CreateDerivedViews()
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srvDesc.Format = Graphics::GetFormat(mFormat);
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Buffer.NumElements = (unsigned int)mElemCount;
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

			if (!mSRV.IsValid())
			{
				mSRV = mRenderer->Heap()->Allocate();
			}
			mRenderer->GetDevice()->CreateShaderResourceView(mResource, &srvDesc, mSRV.mCpuHandle);

			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			uavDesc.Format = Graphics::GetFormat(mFormat);
			uavDesc.Buffer.NumElements = (unsigned int)mElemCount;
			uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

			if (!mUAV.IsValid())
			{
				mUAV = mRenderer->Heap()->Allocate();
			}
			mRenderer->GetDevice()->CreateUnorderedAccessView(mResource, nullptr, &uavDesc, mUAV.mCpuHandle);
		}
	};
}

#endif