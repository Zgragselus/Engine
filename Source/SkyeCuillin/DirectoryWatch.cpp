#include "DirectoryWatch.h"

using namespace SkyeCuillin;

DirectoryWatch::DirectoryWatch(Engine::Log* log, Engine::Constants* options) : Engine::System("DirectoryWatch")
{
	// Run task only once
	EnableUpdater(Engine::Task::BACKGROUND);

	mLog = log;
	mOptions = options;
	mDirTree = nullptr;
}

DirectoryWatch::~DirectoryWatch()
{

}

bool DirectoryWatch::Init()
{
	// Grab directory from config and set state to running
	mDirectory = mOptions->Get<std::string>("Editor.Resources");
	mRunning = true;

	// Open temporary file, this is used to notify watcher thread when application is exitting
	mWatchFile.open((mDirectory + "/tmp.tmp").c_str(), std::ofstream::out | std::ofstream::trunc);

	// Log start of DirectoryWatch
	mLog->Print("DirectoryWatch", "Initializing directory watch for '" + mDirectory + "'");

	return true;
}

void DirectoryWatch::Shutdown()
{
	// Terminate loop in directory watcher
	mRunning = false;

	// Close temporary file and write into it, this needs to be done to unlock watcher attached to 
	// changed files in specified directory. This will unlock it and next state check for mRunning
	// will fail - terminating the watcher thread
	mWatchFile << "Closing Directory Watcher" << std::endl;
	mWatchFile.close();
	mLog->Print("DirectoryWatch", "Shutting down directory watch for '" + mDirectory + "'");

	// Join watcher thread
	mWatchThread.join();

	// Log that directory watching has finished
	mLog->Print("DirectoryWatch", "Finished");
}

void DirectoryWatch::Update()
{
	// On first and only update - initialize the watcher thread
	mWatchThread = std::thread(DirectoryWatch::WatcherThread, this);
}

void DirectoryWatch::WatcherThread(DirectoryWatch* dir)
{
	HANDLE directory;
	FILE_NOTIFY_INFORMATION notifyInfo[1024];
	DWORD bytesReturned;

	// Obtain file handle for directory
	directory = CreateFile(dir->mDirectory.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	// Loop for watcher thread
	while (dir->mRunning)
	{
		// Zero memory
		memset(notifyInfo, 0, sizeof(notifyInfo));

		// Wait for notification on filesystem (for specified directory and subdirectories)
		if (ReadDirectoryChangesW(directory, (LPVOID)&notifyInfo, sizeof(notifyInfo), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, &bytesReturned, NULL, NULL) == 0)
		{
			// Fail in case of error
			dir->mLog->Print("DirectoryWatch", "ReadDirectoryChangesW failed!");
		}
		else
		{
			// Obtain changes and print them out
			std::wstring ws(notifyInfo[0].FileName);
			std::string filename(ws.begin(), ws.end());

			// Notify directory tree about change
			if (dir->mDirTree != nullptr)
			{
				dir->mDirTree->NotifyChange(filename);
			}

			/*switch (notifyInfo[0].Action)
			{
			case FILE_ACTION_ADDED:
			dir->mLog->Print("DirectoryWatch", "File added " + filename);
			printf("ADDED %s\n", filename.c_str());
			break;

			case FILE_ACTION_REMOVED:
			dir->mLog->Print("DirectoryWatch", "File removed " + filename);
			printf("REMOVED %s\n", filename.c_str());
			break;

			case FILE_ACTION_MODIFIED:
			dir->mLog->Print("DirectoryWatch", "File modified " + filename);
			printf("MODIFIED %s\n", filename.c_str());
			break;

			case FILE_ACTION_RENAMED_OLD_NAME:
			dir->mLog->Print("DirectoryWatch", "File renamed " + filename);
			printf("RENAMED_OLD_NAME %s %s\n", filename.c_str());
			break;

			case FILE_ACTION_RENAMED_NEW_NAME:
			dir->mLog->Print("DirectoryWatch", "File renamed " + filename);
			printf("RENAMED_NEW_NAME %s %s\n", filename.c_str());
			break;
			}*/
		}
	}
}

void DirectoryWatch::SetDirectoryTree(DirectoryTree* tree)
{
	mDirTree = tree;
}