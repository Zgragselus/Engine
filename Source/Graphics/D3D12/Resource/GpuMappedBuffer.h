#ifndef __GPU_MAPPED_BUFFER__H__
#define __GPU_MAPPED_BUFFER__H__

#include "GpuResource.h"
#include "../D3DRenderer.h"

namespace Engine
{
	class GpuMappedBuffer : public GpuResource
	{
	protected:
		void* mCpuVirtualAddress;

	public:
		enum Mode
		{
			UPLOAD,
			READBACK
		};

		GpuMappedBuffer();
		~GpuMappedBuffer();

		void Init(D3DRenderer* renderer, unsigned int numElements, unsigned int elementSize, enum Mode mode = UPLOAD);

		void* Map(size_t begin = 0, size_t end = 0);
		void Unmap();

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(unsigned int offset = 0) const { return mGpuVirtualAddress + offset; }
	};
}

#endif