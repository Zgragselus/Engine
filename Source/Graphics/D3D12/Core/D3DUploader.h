#ifndef __D3D_UPLOADER__H__
#define __D3D_UPLOADER__H__

#include "CommandAllocator.h"
#include "CommandList.h"

namespace Engine
{
	class D3DUploader
	{
	public:
		struct MapResult
		{
			void* mCPUAddress;
			size_t mGPUAddress;
			size_t mResourceOffset;
			ID3D12Resource* mResource;
		};

		struct UploadContext
		{
			CommandList* mCommandList;
			void* mCPUAddress;
			size_t mResourceOffset;
			ID3D12Resource* mResource;
		};

	private:
		CommandAllocator* mAllocator;
		CommandList* mList;

	public:
		D3DUploader(ID3D12Device* device)
		{
			mAllocator = new CommandAllocator(device, CommandAllocator::COMMAND_LIST_COPY);
			mList = new CommandList(device, mAllocator, CommandAllocator::COMMAND_LIST_COPY);
		}

		~D3DUploader()
		{
			delete mList;
			delete mAllocator;
		}

		UploadContext ResourceUploadBegin(size_t size)
		{

		}

		void ResourceUploadEnd(UploadContext& context)
		{

		}

		MapResult AcquireTempMemBuffer(size_t size, size_t alignment)
		{

		}
	};
}

#endif