#ifndef __TEXTURE__H__D3D__
#define __TEXTURE__H__D3D__

#include "../Resource/GpuResource.h"
#include "../D3DRenderer.h"
#include "Sampler.h"

namespace Engine
{
	class Texture : public GpuResource
	{
	protected:
		D3DRenderer* mRenderer;

		DescriptorHandle mCpuDesc;
		DescriptorHandle* mUAV;
		unsigned int mMiplevels;
		
		std::string mName;

	public:
		Texture() : GpuResource() {}
		Texture(const std::string& name) : GpuResource(), mName(name) {}
		virtual ~Texture() 
		{
			if (mUAV && mUAV[0].IsValid())
			{
				for (unsigned int i = 0; i < mMiplevels; i++)
				{
					mRenderer->Heap()->Free(mUAV[i]);
				}
			}

			if (mCpuDesc.IsValid())
			{
				mRenderer->Heap()->Free(mCpuDesc);
			}
		}

		void Init(D3DRenderer* renderer, size_t width, size_t height, size_t depth, Graphics::Format format, const void* data, DescriptorHeap* heap = nullptr, bool createUAV = false);
		void InitMipmaps(D3DRenderer* renderer, size_t width, size_t height, size_t depth, size_t miplevels, Graphics::Format format, const void** data, DescriptorHeap* heap = nullptr, bool createUAV = false);

		const DescriptorHandle& GetSRV()
		{
			return mCpuDesc;
		}

		const DescriptorHandle& GetUAV(size_t index = 0)
		{
			return mUAV[index];
		}
		
		const std::string& GetName()
		{
			return mName;
		}

		void SetName(const std::string& name)
		{
			mName = name;
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::Texture")
	};
}

#endif
