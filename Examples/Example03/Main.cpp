#include "Main.h"
#include "Graphics/Graphics.h"

class Main : public Engine::System
{
private:
	Engine::Window* mWindow;

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

		static Engine::EventChannel chan;
		chan.Add<Engine::Keyboard::KeyPressed>(*this);
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
		mWindow->Paint();
	}

	void Handle(const Engine::Keyboard::KeyPressed& kp)
	{
		static Engine::EventChannel chan;

		if (kp.mKey == Engine::Keyboard::KEY_ESCAPE)
		{
			chan.Broadcast(Engine::Scheduler::StopEvent());
		}
	}
};

int main()
{
	Engine::Log* log = new Engine::Log();
	log->AddOutput(new std::ofstream("Output.log"), Engine::Log::LOG_DEFAULT);

	Engine::Constants* options = new Engine::Constants(log, "../Data/Example03/Config.conf");

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