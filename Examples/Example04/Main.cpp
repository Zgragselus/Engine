#include "Main.h"
//#include "Graphics/D3D12/DX12.h"
//#include "Graphics/D3D12/DX12SwapChain.h"

class Main : public Engine::System
{
private:
	Engine::Window* mWindow;
	/*Engine::Graphics::DX12* mDX;
	Engine::Graphics::DXSwapChain* mDXSwapChain;*/

public:
	struct KeyPressed
	{
		KeyPressed(int c) :
			mKey(c)
		{
		}

		int mKey;
	};

	Main(Engine::Log* log, Engine::Constants* options) : Engine::System("Main")
	{
		EnableUpdater();

		mWindow = new Engine::Window(log, options);
		mWindow->Create();

		/*mDX = new Engine::Graphics::DX12(log, options);
		mDXSwapChain = new Engine::Graphics::DXSwapChain(mWindow, log, options);*/

		static Engine::EventChannel chan;
		chan.Add<Engine::Keyboard::KeyPressed>(*this);
		chan.Add<Engine::Window::Resize>(*this);
	}

	virtual ~Main()
	{
		delete mWindow;
	}

	virtual bool Init()
	{
		return true;
	}

	virtual void Shutdown()
	{
		mWindow->Destroy();
	}

	virtual void Update()
	{
		mWindow->Process();

		/*mDX->BeginFrame();
		mDXSwapChain->BeginFrame();

		int w;
		int h;
		this->mWindow->GetSize(&w, &h);

		D3D12_VIEWPORT viewport = D3D12_VIEWPORT();
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MaxDepth = 1.0f;
		viewport.Width = w;
		viewport.Height = h;

		D3D12_RECT rect = D3D12_RECT();
		rect.right = w;
		rect.bottom = h;

		//mDX->mCmdList->SetGraphicsRootSignature(this->mRootSignature->Get());
		mDX->mCmdList->RSSetViewports(1, &viewport);
		mDX->mCmdList->RSSetScissorRects(1, &rect);

		mDX->mCmdList->OMSetRenderTargets(1, &mDXSwapChain->BackBuffer().RTV.mCpuHandle, FALSE, nullptr);

		float color[4];
		color[0] = 1.0f;
		color[1] = 0.0f;
		color[2] = 0.0f;
		color[3] = 1.0f;
		mDX->mCmdList->ClearRenderTargetView(mDXSwapChain->BackBuffer().RTV.mCpuHandle, color, 0, NULL);

		mDXSwapChain->EndFrame();
		mDX->EndFrame(mDXSwapChain->D3DSwapChain(), mDXSwapChain->VSyncIntervals());*/
	}

	void Handle(const Engine::Keyboard::KeyPressed& kp)
	{
		static Engine::EventChannel chan;

		if (kp.mKey == Engine::Keyboard::KEY_ESCAPE)
		{
			chan.Broadcast(Engine::Scheduler::StopEvent());
		}
	}

	void Handle(const Engine::Window::Resize& r)
	{
	}
};

int main()
{
	Engine::Log* log = new Engine::Log();
	log->AddOutput(new std::ofstream("Output.log"), Engine::Log::LOG_DEFAULT);

	Engine::Constants* options = new Engine::Constants(log, "../Data/Example04/Config.conf");

	Engine::Input* input = new Engine::Input();

	Main* m = new Main(log, options);

	Engine::Core::Instance()->Add(input);
	Engine::Core::Instance()->Add(m);
	Engine::Core::Instance()->SetLog(log);
	Engine::Core::Instance()->SetOptions(options);
	Engine::Core::Instance()->Run();
	Engine::Core::Instance()->Dispose();

	delete m;
	delete input;
	delete options;
	delete log;

	Engine::MemoryDump(std::cout);

	return 0;
}