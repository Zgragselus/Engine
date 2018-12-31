#ifndef __DIRECTORY_WATCH__H__
#define __DIRECTORY_WATCH__H__

#include "DirectoryTree.h"

namespace SkyeCuillin
{
	class DirectoryWatch : public Engine::System
	{
	private:
		Engine::Constants* mOptions;
		Engine::Log* mLog;
		std::string mDirectory;
		bool mRunning;
		std::thread mWatchThread;
		std::ofstream mWatchFile;
		DirectoryTree* mDirTree;

	public:
		DirectoryWatch(Engine::Log* log, Engine::Constants* options);

		virtual ~DirectoryWatch();

		virtual bool Init();

		virtual void Shutdown();

		virtual void Update();

		static void WatcherThread(DirectoryWatch* dir);

		void SetDirectoryTree(DirectoryTree* tree);
	};
}

#endif