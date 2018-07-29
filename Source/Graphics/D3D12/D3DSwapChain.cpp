#include "D3DSwapChain.h"

using namespace Engine;

D3DSwapChain::D3DSwapChain(D3DRenderer* renderer, Window* window, Log* log, Constants* constants)
{
	mWindow = window;
	mLog = log;
	mConstants = constants;
	mRenderer = renderer;

	// Before we can initialize swap chain, refresh rate denominator and numerator have to be obtained along with 
	// refresh rate from GPU

	// Create interface factory
	IDXGIFactory4* factory;
	HRESULT result = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&factory);
	if (FAILED(result))
	{
		MessageBox(window->GetWindow(), "Error: CreateDXGIFactory1 failed!", "DirectX Device Failure", MB_OK);
		return;
	}

	// Use the factory to enum adapters
	IDXGIAdapter* adapter;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		MessageBox(window->GetWindow(), "Error: EnumAdapters failed!", "DirectX Device Failure", MB_OK);
		return;
	}

	// Enumerate primary adapter output
	IDXGIOutput* adapterOutput;
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		MessageBox(window->GetWindow(), "Error: EnumOutputs failed!", "DirectX Device Failure", MB_OK);
		return;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	unsigned int numModes;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		MessageBox(window->GetWindow(), "Error: GetDisplayModeList failed!", "DirectX Device Failure", MB_OK);
		return;
	}

	// Create a list to hold all possible display modes	
	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		MessageBox(window->GetWindow(), "Error: Out of memory!", "DirectX Device Failure", MB_OK);
		return;
	}

	// Obtain the display mode list
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		MessageBox(window->GetWindow(), "Error: GetDisplayModeList failed!", "DirectX Device Failure", MB_OK);
		return;
	}

	// Now go through all display modes and find the one matching the screen size, when a match
	// is found, store numerator and denominator
	unsigned int numerator = 0;
	unsigned int denominator = 0;
	for (unsigned int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Height == (unsigned int)constants->Get<int>("Window.Size.Height"))
		{
			if (displayModeList[i].Width == (unsigned int)constants->Get<int>("Window.Size.Width"))
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Also store video card memory and name
	DXGI_ADAPTER_DESC adapterDesc;
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		MessageBox(window->GetWindow(), "Error: GetDesc failed!", "DirectX Device Failure", MB_OK);
		return;
	}

	char mGpuDescription[128];
	unsigned int mGpuMemory;
	mGpuMemory = (unsigned int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	size_t stringLength;
	wcstombs_s(&stringLength, mGpuDescription, 128, adapterDesc.Description, 128);

	log->Print("Graphics.D3DRenderer", std::string("Video card - ") + std::string(mGpuDescription));
	log->Print("Graphics.D3DRenderer", std::string("Available video memory - ") + std::to_string(mGpuMemory) + std::string("MB"));

	// Release adapter and display mode list
	delete[] displayModeList;
	adapterOutput->Release();
	adapter->Release();

	// As we have refresh rate numerator and denominator, it is possible to generate a swap chain now

	// Set swap chain description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferCount = 2;															// Double buffering
	swapChainDesc.BufferDesc.Width = constants->Get<int>("Window.Size.Width");				// Width
	swapChainDesc.BufferDesc.Height = constants->Get<int>("Window.Size.Height");			// Height
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;							// Regular RGBA8 format for backbuffer
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;							// Backbuffers are render output
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;								// Discard previous content on swap buffers
	swapChainDesc.OutputWindow = window->GetWindow();										// Output window to which we render 
	swapChainDesc.Windowed = constants->Get<int>("Window.Fullscreen") == 0 ? true : false;	// Set fullscreen or window rendering
																							// In case of vsync enabled, set numerator and denominator to apropriate values
	mVsync = constants->Get<int>("Graphics.DirectX.VSync") == 1 ? true : false;
	if (mVsync)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	swapChainDesc.SampleDesc.Count = 1;														// Multisample sample buffers count
	swapChainDesc.SampleDesc.Quality = 0;													// Multisample sample buffers quality
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;						// Don't specify scaling
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;		// Don't specify scanline order
	swapChainDesc.Flags = 0;																// No flags

																							// Create swap chain
	IDXGISwapChain* swapChain;
	//result = factory->CreateSwapChain(mRenderer->GetCommandQueue()->Get(), &swapChainDesc, &swapChain);
	result = factory->CreateSwapChain(mRenderer->mCommandManager->GetGraphicsQueue()->GetCommandQueue(), &swapChainDesc, &swapChain);
	if (FAILED(result))
	{
		MessageBox(window->GetWindow(), "Error: CreateSwapChain failed!", "DirectX Device Failure", MB_OK);
		return;
	}

	// Upgrade IDXGISwapChain to IDXGISwapChain3 allowing for extended functionality
	result = swapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&mSwapChain);
	if (FAILED(result))
	{
		MessageBox(window->GetWindow(), "Error: QueryInterface failed!", "DirectX Device Failure", MB_OK);
		return;
	}

	// Clear pointer to original swap chain (but don't release it)
	swapChain = 0;

	// Release DXGI facotry
	factory->Release();

	// Setup render target views for both back buffers. To create the views we need to first create a descriptor
	// heap to hold back buffers in memory, after that we can get a handle to that memory and create a view to
	// that memory location. This is common for all DirectX 12 resource binding

	// Initialize descriptor for render target view heap for 2 back buffers
	//mRTVHeap = new DescriptorHeap(mDevice, Engine::DescriptorHeap::RENDER_TARGET_VIEW, 2);

	// Create a render target view for both back buffers
	for (unsigned int i = 0; i < 2; i++)
	{
		mBackBuffer[i] = new ColorBuffer(mRenderer, 0.0f, 0.0f, 0.0f, 1.0f);
		mBackBuffer[i]->CreateFromSwapChain(mSwapChain, i);
	}

	// Get initial index to which buffer is current backbuffer
	mBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

	int width, height;
	mWindow->GetSize(&width, &height);

	mDepthBuffer = new DepthBuffer(mRenderer, 1.0f, 0);
	mDepthBuffer->Create(width, height, Graphics::D32F);
}

D3DSwapChain::~D3DSwapChain()
{
	mSwapChain->SetFullscreenState(false, NULL);
	delete mBackBuffer[0];
	delete mBackBuffer[1];
	delete mDepthBuffer;
	mSwapChain->Release();
}

void D3DSwapChain::Resize(int width, int height)
{
	mBackBuffer[0]->Release();
	mBackBuffer[1]->Release();

	mSwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	mBackBuffer[0]->CreateFromSwapChain(mSwapChain, 0);
	mBackBuffer[1]->CreateFromSwapChain(mSwapChain, 1);

	mBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

	mDepthBuffer->Release();
	mDepthBuffer->Create(width, height, Graphics::D32F);

	mRenderer->Flush();
}