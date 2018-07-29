#ifndef __PIXEL_BUFFER__H__
#define __PIXEL_BUFFER__H__

#include "GpuResource.h"
#include "../Core/Common.h"

namespace Engine
{
	class PixelBuffer : public GpuResource
	{
	protected:
		unsigned int mWidth;
		unsigned int mHeight;
		unsigned int mArraySize;
		Graphics::Format mFormat;

		static DXGI_FORMAT GetBaseFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetUAVFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetDepthFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetStencilFormat(DXGI_FORMAT format);
		D3D12_RESOURCE_DESC DescTex2D(unsigned int width, unsigned int height, unsigned int depthOrArraySize, unsigned int numMips, Graphics::Format format, unsigned int flags);
		void CreateTextureResource(ID3D12Device* device, const D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE clear);

	public:
		PixelBuffer() : GpuResource()
		{
			mWidth = mHeight = 0;
		}

		PixelBuffer(D3D12_RESOURCE_STATES state) : GpuResource(NULL, state)
		{
			mWidth = mHeight = 0;
		}

		virtual ~PixelBuffer()
		{

		}

		unsigned int GetWidth() const { return mWidth; }
		unsigned int GetHeight() const { return mHeight; }
		unsigned int GetDepth() const { return mArraySize; }
		const Graphics::Format& GetFormat() const { return mFormat; }
	};
}

#endif 