#ifndef __DESCRIPTOR_HANDLE__H__
#define __DESCRIPTOR_HANDLE__H__

#include <d3d12.h>

namespace Engine
{
	class DescriptorHandle
	{
	public:
		D3D12_CPU_DESCRIPTOR_HANDLE mCpuHandle = {};
		D3D12_GPU_DESCRIPTOR_HANDLE mGpuHandle = {};

		bool IsValid() const
		{
			return mCpuHandle.ptr != 0;
		}

		bool ShaderVisible() const
		{
			return mGpuHandle.ptr != 0;
		}
	};
}

#endif