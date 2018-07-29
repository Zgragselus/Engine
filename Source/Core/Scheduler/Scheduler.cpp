///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Scheduler.cpp
//
// Following file contains definition of Scheduler system for an engine.
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Scheduler.h"
#include "Core/Task/Task.h"
#include "Core/Event/EventChannel.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Declaration section

using namespace Engine;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

/// <summary>
/// Dummy task class
/// </summary>
class BackgroundDummyTask : public Task
{
public:
	/// <summary>
	/// Constructor
	/// </summary>
	BackgroundDummyTask() : Task(BACKGROUND_REPEATING)
	{
	}

	/// <summary>
	/// Destructor
	/// </summary>
	virtual ~BackgroundDummyTask()
	{
	}

	/// <summary>
	/// Run dummy task
	/// </summary>
	virtual void Run()
	{
		std::this_thread::sleep_for(std::chrono::microseconds(10));
		//std::this_thread::yield();
	}
};

BackgroundDummyTask mBackgroundDummyTask;	// Dummy task for background worker threads

/// <summary>
/// Friend worker thread function
/// </summary>
/// <param name="arg">Pointer to scheduler</param>
int Engine::Scheduler_Worker(void* arg)
{
	Scheduler* sched = (Scheduler*)arg;

	Task* task;

	while (sched->mRunning)
	{
		task = sched->mBackgroundTasks.pop();
		sched->Execute(task);

		if (task->GetTaskFlags() & Task::FRAME_SYNC)
		{
			std::unique_lock<std::mutex> lock(sched->mSyncMutex);
			sched->mNumTasksToWaitFor -= 1;
			lock.unlock();

			sched->mCondition.notify_all();
		}

		std::this_thread::sleep_for(std::chrono::microseconds(10));
		//std::this_thread::yield();
	}

	return 0;
}

/// <summary>
/// Constructor for scheduler
/// </summary>
/// <param name="numThreads">How many threads should be used by scheduler for parallel tasks</param>
Scheduler::Scheduler(unsigned int numThreads)
{
	// Allocate and initialize
	mNumThreads = numThreads;
	mThreads = new std::thread[mNumThreads];

	mWriteList = 0;
	mReadList = 1;

	mNumTasksToWaitFor = 0;
}

/// <summary>
/// Destructor
/// </summary>
Scheduler::~Scheduler()
{
	// Join all worker threads
	for (unsigned int i = 0; i < mNumThreads; i++)
	{
		mThreads[i].join();
	}

	delete[] mThreads;
}

/// <summary>
/// Add new task into the scheduler
/// </summary>
/// <param name="task">Pointer to task to add</param>
void Scheduler::Add(Task* task)
{
	// Determine where it belongs and push it to correct list
	unsigned int flags = task->GetTaskFlags();

	if (flags & Task::THREAD_SAFE)
	{
		if (flags & Task::FRAME_SYNC)
		{
			mSyncTasks.push(task);
		}
		else
		{
			mBackgroundTasks.push(task);
		}
	}
	else
	{
		mTaskList[mWriteList].push(task);
	}
}

/// <summary>
/// Start scheduling (call once at the initialization of game engine)
/// </summary>
void Scheduler::Start()
{
	mRunning = true;

	EventChannel chan;

	// Add events into channel
	chan.Add<Task::TaskCompleted>(*this);
	chan.Add<StopEvent>(*this);

	// Add dummy task for background tasks
	Add(&mBackgroundDummyTask);

	// Start threads
	for (unsigned int i = 0; i < mNumThreads; i++)
	{
		mThreads[i] = std::thread(Scheduler_Worker, this);
	}

	// Application loop
	while (mRunning)
	{
		// Process synchronous tasks
		if (!mTaskList[mReadList].empty())
		{
			Task* t = mTaskList[mReadList].pop();
			Execute(t);
		}
		// Synchronize and swap read/write buffers
		else
		{
			Synchronize();

			unsigned int tmp = mReadList;
			mReadList = mWriteList;
			mWriteList = tmp;
		}

		std::this_thread::sleep_for(std::chrono::microseconds(10));
		//std::this_thread::yield();
	}
}

/// <summary>
/// Stop scheduling (call once at point where game engine is shutdown)
/// </summary>
void Scheduler::Stop()
{
	mRunning = false;
}

/// <summary>
/// Handle stop event
/// </summary>
void Scheduler::Handle(const StopEvent&)
{
	Stop();
}

/// <summary>
/// Handle task completed event
/// </summary>
void Scheduler::Handle(const Task::TaskCompleted& tc)
{
	// If task is repeating, re-push it back
	if (tc.mTask->GetTaskFlags() & Task::REPEATING)
	{
		if (tc.mTask->GetTaskFlags() & Task::BACKGROUND)
		{
			Add(tc.mTask);
		}
		else
		{
			Add(tc.mTask);
		}
	}
}

/// <summary>
/// Execute single task
/// </summary>
void Scheduler::Execute(Task* task)
{
	EventChannel chan;

	// Broadcast TaskBeginning, run the task and broadcast TaskCompleted
	chan.Broadcast(Task::TaskBeginning(task));
	task->Run();
	chan.Broadcast(Task::TaskCompleted(task));
}

/// <summary>
/// Synchronize the scheduler
/// </summary>
void Scheduler::Synchronize()
{
	std::unique_lock<std::mutex> lock(mSyncMutex);

	// Wait until all sync tasks are finished
	while (mNumTasksToWaitFor > 0)
	{
		mCondition.wait(lock);
	}

	mNumTasksToWaitFor = mSyncTasks.size();

	// Push all sync tasks into background tasks
	while (!mSyncTasks.empty())
	{
		mBackgroundTasks.push(mSyncTasks.pop());
	}

	lock.unlock();
}
