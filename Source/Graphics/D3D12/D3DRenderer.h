///////////////////////////////////////////////////////////////////////////////////////////////////
//
// D3DRenderer.h
//
// DirectX 12 renderer base class
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DIRECTX12__RENDERER__H__
#define __DIRECTX12__RENDERER__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Window/Window.h"
#include "Core/Log/Log.h"
#include "Core/Constants/Constants.h"
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include "Core/D3DShader.h"
#include "Pipeline/RasterizerState.h"
#include "Pipeline/BlendState.h"
#include "Pipeline/PipelineState.h"
#include "Heap/DescriptorHeap.h"
#include "Core/CommandQueue.h"
#include "Core/CommandAllocator.h"
#include "Core/CommandList.h"
#include "Core/ResourceBarrier.h"
#include "Core/RootSignature.h"
#include "Core/Fence.h"
#include "Core/Context.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// DirectX 12 renderer class
	/// </summary>
	class D3DRenderer
	{
		friend class D3DSwapChain;

	private:
		Log* mLog;				// Pointer to log instance
		Window* mWindow;		// Pointer to window instance
		Constants* mConstants;	// Pointer to options (constants instance)

		ID3D12Device* mDevice;

		//CommandQueue* mQueue;
		CommandListManager* mCommandManager;
		ContextManager* mContextManager;

		DescriptorHeap* mRTVHeap;
		DescriptorHeap* mDSVHeap;
		DescriptorHeap* mSRVCBVUAVHeap;
		
		Fence* mFence;
		size_t mRenderLatency;
		size_t mCurrentCpuFrame;
		size_t mCurrentGpuFrame;
		size_t mCurrentFrameIdx;

		char mGpuDescription[128];
		unsigned int mGpuMemory;
				
	public:
		/// <summary>
		/// Renderer constructor
		/// </summary>
		/// <param name="window">Pointer to window into which the renderer will be bound</param>
		/// <param name="log">Pointer to log (error and debug output)</param>
		/// <param name="constants">Pointer to options (constants instance)</param>
		D3DRenderer(Window* window, Log* log, Constants* constants);

		/// <summary>
		/// Renderer destructor
		/// </summary>
		~D3DRenderer();

		/// <summary>
		/// Render frame
		/// </summary>
		bool Render();

		inline GraphicsContext* GetGraphicsContext()
		{
			return (GraphicsContext*)(mContextManager->AllocateContext(mDevice, CommandAllocator::COMMAND_LIST_DIRECT));
		}

		inline ComputeContext* GetComputeContext()
		{
			return (ComputeContext*)(mContextManager->AllocateContext(mDevice, CommandAllocator::COMMAND_LIST_COMPUTE));
		}

		inline void ReleaseGraphicsContext(GraphicsContext* context)
		{
			mContextManager->FreeContext(context);
		}

		inline void ReleaseComputeContext(ComputeContext* context)
		{
			mContextManager->FreeContext(context);
		}
		
		inline ID3D12Device* GetDevice()
		{
			return mDevice;
		}

		/*inline void ExecuteCommandList(CommandList* list)
		{
			ID3D12CommandList* ptr[1];
			ptr[0] = (ID3D12CommandList*)list->Get();
			this->mQueue->Get()->ExecuteCommandLists(1, ptr);
		}*/

		/*inline DescriptorHandle GetBackbufferView()
		{
			return this->mRTVHeap->Get(this->mBufferIndex);
		}

		inline DescriptorHandle GetDepthStencilView()
		{
			return this->mDSVHeap->Get(0);
		}

		inline ID3D12Resource* GetBackbuffer()
		{
			return this->mBackbufferRenderTarget[this->mBufferIndex];
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

			this->Flush();

			this->mBufferIndex = this->mSwapChain->GetCurrentBackBufferIndex();
		}*/

		/// <summary>
		/// Synchronize GPU
		/// </summary>
		void Flush();

		inline DescriptorHeap* HeapRTV()
		{
			return mRTVHeap;
		}

		inline DescriptorHeap* HeapDSV()
		{
			return mDSVHeap;
		}

		inline DescriptorHeap* Heap()
		{
			return mSRVCBVUAVHeap;
		}

		/*inline CommandQueue* GetCommandQueue()
		{
			return mQueue;
		}*/

		ALIGNED_NEW_DELETE("Engine::Graphics::Context")
	};
}

#endif