#ifndef __D3DDEVICE__H__
#define __D3DDEVICE__H__

#include <d3d12.h>
#include "Core/Log/Log.h"

namespace Engine
{
	class D3DDevice
	{
	private:
		ID3D12Device* mDevice;

	public:
		inline D3DDevice(Log* mLog)
		{

		}

		inline ~D3DDevice()
		{
			mDevice->Release();
		}

		inline ID3D12Device* Get()
		{
			return mDevice;
		}
	};
}

#endif