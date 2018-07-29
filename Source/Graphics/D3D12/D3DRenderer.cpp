#include "D3DRenderer.h"

using namespace Engine;

/// <summary>
/// Renderer constructor
/// </summary>
/// <param name="window">Pointer to window into which the renderer will be bound</param>
/// <param name="log">Pointer to log (error and debug output)</param>
/// <param name="constants">Pointer to options (constants instance)</param>
D3DRenderer::D3DRenderer(Window* window, Log* log, Constants* constants)
{
	mWindow = window;
	mLog = log;
	mConstants = constants;

	mRenderLatency = (size_t)constants->Get<int>("Graphics.DirectX.RenderLatency");

	// When in debug mode, enabel debug layer
#ifdef _DEBUG
	ID3D12Debug* dbg;
	D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&dbg);
	dbg->EnableDebugLayer();

	//ID3D12Debug1* dbg1 = (ID3D12Debug1*)dbg;
	//dbg->QueryInterface(__uuidof(ID3D12Debug1), (void**)&dbg1);
	//dbg1->SetEnableGPUBasedValidation(TRUE);
	//dbg1->Release();

	dbg->Release();
#endif

	// First we need to create DirectX 12 device
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0;
	HRESULT result = D3D12CreateDevice(NULL, featureLevel, __uuidof(ID3D12Device), (void**)&this->mDevice);
	if (FAILED(result))
	{
		mLog->Print("Engine::Graphics::Device", std::string("Error: Could not create DirectX 12 context with feature level 12_0. The default GPU does not support DirectX 12!"));
		return;
	}
	
	// Initialize command queue
	//this->mQueue = new CommandQueue(this->mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_QUEUE_PRIORITY_NORMAL, D3D12_COMMAND_QUEUE_FLAG_NONE);

	mCommandManager = new CommandListManager(mDevice);
	mContextManager = new ContextManager(mCommandManager);

	// Initialize heaps
	mRTVHeap = new DescriptorHeap(mDevice, DescriptorHeap::RENDER_TARGET_VIEW, mConstants->Get<int>("Graphics.DirectX.HeapSizeRTV"));
	mDSVHeap = new DescriptorHeap(mDevice, DescriptorHeap::DEPTH_STENCIL_VIEW, mConstants->Get<int>("Graphics.DirectX.HeapSizeDSV"));
	mSRVCBVUAVHeap = new DescriptorHeap(mDevice, DescriptorHeap::CBV_SRV_UAV, mConstants->Get<int>("Graphics.DirectX.HeapSizeSRVCBVUAV"), true);

	// The last thing that needs to be created is the fence. Fence is used as a signaling mechanism when GPU is completely 
	// done rendering the command list submitted via command queue. GPU and CPU sync is completely up to us.
	/*this->mFence = new Fence(this->mDevice, size_t(-1));
	this->mCurrentCpuFrame = 0;
	this->mCurrentGpuFrame = 0;
	this->mCurrentFrameIdx = this->mCurrentCpuFrame % this->mRenderLatency;

	this->Flush();*/
}

/// <summary>
/// Renderer destructor
/// </summary>
D3DRenderer::~D3DRenderer()
{
	delete mCommandManager;
	delete mContextManager;
	delete this->mFence;
	delete mRTVHeap;
	delete mDSVHeap;
	delete mSRVCBVUAVHeap;
	this->mDevice->Release();
}

/// <summary>
/// Render frame
/// </summary>
bool D3DRenderer::Render()
{
	return true;
}

void D3DRenderer::Flush()
{
	/*mCurrentCpuFrame++;
	mFence->Signal(mQueue->Get(), mCurrentCpuFrame);
	uint64_t gpuDiff = mCurrentCpuFrame - mCurrentGpuFrame;
	if (gpuDiff >= mRenderLatency)
	{
		mFence->Wait(mCurrentGpuFrame + 1);
		mCurrentGpuFrame++;
	}

	mCurrentFrameIdx = mCurrentCpuFrame % mRenderLatency;*/
}