#ifndef __DEPTH_BUFFER__H__
#define __DEPTH_BUFFER__H__

#include "PixelBuffer.h"
#include "../Heap/DescriptorHandle.h"

namespace Engine
{
	class D3DRenderer;

	class DepthBuffer : public PixelBuffer
	{
	protected:
		D3DRenderer* mRenderer;
		float mClearDepth;
		unsigned int mClearStencil;
		DescriptorHandle mDSV[4];
		DescriptorHandle mDepthSRV;
		DescriptorHandle mStencilSRV;
		DescriptorHandle mDSVCube[6];
		unsigned int mFragmentCount;
		bool mIsCubemap;

		void CreateDerivedViews(DXGI_FORMAT format);

	public:
		DepthBuffer(D3DRenderer* renderer, float clearDepth, unsigned int clearStencil) : PixelBuffer(D3D12_RESOURCE_STATE_DEPTH_WRITE)
		{
			mRenderer = renderer;
			mClearDepth = clearDepth;
			mClearStencil = clearStencil;

			mDSV[0].mCpuHandle.ptr = 0;
			mDSV[1].mCpuHandle.ptr = 0;
			mDSV[2].mCpuHandle.ptr = 0;
			mDSV[3].mCpuHandle.ptr = 0;
			mDepthSRV.mCpuHandle.ptr = 0;
			mStencilSRV.mCpuHandle.ptr = 0;

			mDSVCube[0].mCpuHandle.ptr = 0;
			mDSVCube[1].mCpuHandle.ptr = 0;
			mDSVCube[2].mCpuHandle.ptr = 0;
			mDSVCube[3].mCpuHandle.ptr = 0;
			mDSVCube[4].mCpuHandle.ptr = 0;
			mDSVCube[5].mCpuHandle.ptr = 0;

			mFragmentCount = 1;
		}

		virtual ~DepthBuffer()
		{
			Release(true);
		}

		void Create(unsigned int width, unsigned int height, Graphics::Format format, bool cubemap = false, unsigned int msaaSamples = 1, unsigned int msaaQuality = 0);

		void Release(bool dtor = false);

		const DescriptorHandle& GetDSV() const { return mDSV[0]; }
		const DescriptorHandle& GetDSV_DepthReadOnly() const { return mDSV[1]; }
		const DescriptorHandle& GetDSV_StencilReadOnly() const { return mDSV[2]; }
		const DescriptorHandle& GetDSV_ReadOnly() const { return mDSV[3]; }
		const DescriptorHandle& GetDepthSRV() const { return mDepthSRV; }
		const DescriptorHandle& GetStencilSRV() const { return mStencilSRV; }
		const DescriptorHandle& GetDSVCube(unsigned int i) const { return mDSVCube[i]; }

		float GetClearDepth() const { return mClearDepth; }
		uint32_t GetClearStencil() const { return mClearStencil; }
	};
}

#endif