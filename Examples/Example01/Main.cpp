#include "Main.h"

class Main : public Engine::System
{
public:
	struct KeyPressed
	{
		KeyPressed(int c) :
			mKey(c)
		{
		}

		int mKey;
	};

	Main() : Engine::System("Main")
	{
		EnableUpdater();

		static Engine::EventChannel chan;
		chan.Add<KeyPressed>(*this);
	}

	virtual ~Main()
	{

	}

	virtual bool Init()
	{
		std::cout << "'p' to print memory (Only debug mode), 'x' to exit; both followed by return." << std::endl;
		return true;
	}

	virtual void Shutdown()
	{
	}

	virtual void Update()
	{
		static Engine::EventChannel chan;
		chan.Broadcast(KeyPressed(std::getchar()));
	}

	void Handle(const KeyPressed& kp)
	{
		static Engine::EventChannel chan;

		if (kp.mKey == 'x')
		{
			chan.Broadcast(Engine::Scheduler::StopEvent());
		}
		else if (kp.mKey == 'p')
		{
			Engine::MemoryDump(std::cout);
		}
	}
};

int main()
{
	Engine::Log* log = new Engine::Log();
	log->AddOutput(new std::ofstream("Output.log"), Engine::Log::LOG_DEFAULT);

	Main* m = new Main();

	Engine::Core::Instance()->Add(m);
	Engine::Core::Instance()->Run();
	Engine::Core::Instance()->Dispose();

	delete m;
	delete log;

	Engine::MemoryDump(std::cout);

	std::cout << "Press return to exit..." << std::endl;
	std::getchar();
	std::getchar();
	
	return 0;
}