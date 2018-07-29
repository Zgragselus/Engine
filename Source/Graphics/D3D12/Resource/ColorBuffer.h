#ifndef __COLOR_BUFFER__H__
#define __COLOR_BUFFER__H__

#include <dxgi1_5.h>
#include "PixelBuffer.h"
#include "../Heap/DescriptorHandle.h"

namespace Engine
{
	class D3DRenderer;

	class ColorBuffer : public PixelBuffer
	{
	protected:
		float mClearColor[4];
		D3DRenderer* mRenderer;
		DescriptorHandle mSRV;
		DescriptorHandle mRTV;
		DescriptorHandle mRTVCube[6];
		DescriptorHandle mUAV[14];
		unsigned int mNumMipMaps;
		unsigned int mFragmentCount;
		unsigned int mSampleCount;
		bool mIsCubemap;

		inline unsigned int ComputeNumMips(unsigned int width, unsigned int height)
		{
			unsigned int highBit;
			_BitScanReverse((unsigned long*)&highBit, width | height);
			return highBit + 1;
		}

		void CreateDerivedViews(ID3D12Device* device, DXGI_FORMAT format, unsigned int arraySize, unsigned int numMips);

	public:
		ColorBuffer(D3DRenderer* renderer, float r, float g, float b, float a)
		{
			mRenderer = renderer;
			mClearColor[0] = r;
			mClearColor[1] = g;
			mClearColor[2] = b;
			mClearColor[3] = a;
			mNumMipMaps = 0;
			mFragmentCount = 1;
			mSampleCount = 1;
		}

		virtual ~ColorBuffer()
		{
			Release(true);
		}

		void Release(bool dtor = false);

		void CreateFromSwapChain(IDXGISwapChain3* swapChain, unsigned int buffer);

		void Create(unsigned int width, unsigned int height, unsigned int numMips, Graphics::Format format, bool cubemap = false, unsigned int msaaSamples = 1, unsigned int msaaQuality = 0, D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN);

		const DescriptorHandle& GetRTV() const { return mRTV; }
		const DescriptorHandle& GetRTVCube(unsigned int i) const { return mRTVCube[i]; }
		const DescriptorHandle& GetSRV() const { return mSRV; }
		const DescriptorHandle& GetUAV(size_t index = 0) { return mUAV[index]; }

		inline unsigned int GetNumMipMaps()
		{
			return mNumMipMaps;
		}
	};
}

#endif