#ifndef __D3D_SWAP_CHAIN__H__
#define __D3D_SWAP_CHAIN__H__

#include "D3DRenderer.h"
#include "Resource/DepthBuffer.h"
#include "Resource/ColorBuffer.h"

namespace Engine
{
	class D3DSwapChain
	{
	private:
		Log* mLog;						// Pointer to log instance
		Window* mWindow;				// Pointer to window instance
		Constants* mConstants;			// Pointer to options (constants instance)
		D3DRenderer* mRenderer;
		IDXGISwapChain3* mSwapChain;
		//DescriptorHandle mBackbufferDescriptor[2];
		//ID3D12Resource* mBackbufferRenderTarget[2];
		//DescriptorHandle mDepthStencilDescriptor;
		//ID3D12Resource* mDepthStencil;
		DepthBuffer* mDepthBuffer;
		ColorBuffer* mBackBuffer[2];
		unsigned int mBufferIndex;
		bool mVsync;

	public:
		D3DSwapChain(D3DRenderer* renderer, Window* window, Log* log, Constants* constants);
		~D3DSwapChain();
		void Resize(int width, int height);
		/*inline DescriptorHandle GetBackbufferView()
		{
			return mBackbufferDescriptor[mBufferIndex];
		}*/

		inline DepthBuffer* GetDepthBuffer()
		{
			return mDepthBuffer;
		}

		inline ColorBuffer* GetBackBuffer()
		{
			return mBackBuffer[mBufferIndex];
		}

		inline void SwapBuffers()
		{
			// Finally present the back buffer to the screen since rendering is complete.
			if (this->mVsync)
			{
				// Lock to screen refresh rate.
				mSwapChain->Present(1, 0);
			}
			else
			{
				// Present as fast as possible.
				mSwapChain->Present(0, 0);
			}

			mRenderer->Flush();

			mBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
		}
	};
}

#endif