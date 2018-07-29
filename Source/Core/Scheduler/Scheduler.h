///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Schedular.h
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

#ifndef __SCHEDULER__H__
#define __SCHEDULER__H__

#define ENGINE_THREADS 4 

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/DllDef.h"
#include "Core/Log/Log.h"
#include "Core/Task/Task.h"
#include "Core/Memory/Memory.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	class Scheduler
	{
	private:
		/// <summary>
		/// Concurrent queue class, e.g. thread safe queue
		/// </summary>
		template <typename T>
		class concurrent_queue
		{
		private:
			std::queue<T> mQueue;					// Queue
			mutable std::mutex mMutex;				// Mutex for handling multiple accessing threads
			std::condition_variable mCondition;		// Conditional variable for signaling

		public:
			/// <summary>
			/// Constructor, allocates necessary attributes
			/// </summary>
			concurrent_queue()
			{
			}

			/// <summary>
			/// Destructor
			/// </summary>
			~concurrent_queue()
			{
			}

			/// <summary>
			/// Is queue empty?
			/// </summary>
			bool empty() const
			{
				std::unique_lock<std::mutex> lock(mMutex);
				bool result = mQueue.empty();
				return result;
			}

			/// <summary>
			/// Size of queue (number of items)
			/// </summary>
			size_t size() const
			{
				std::unique_lock<std::mutex> lock(mMutex);
				size_t result = mQueue.size();
				return result;
			}

			/// <summary>
			/// Push item into queue
			/// </summary>
			/// <param name="value">Item to push into queue</param>
			void push(const T& value)
			{
				std::unique_lock<std::mutex> lock(mMutex);
				mQueue.push(value);
				lock.unlock();
				mCondition.notify_all();
			}

			/// <summary>
			/// Try to pop item from queue, return whether succeeded
			/// </summary>
			/// <param name="result">Reference to target item</param>
			bool try_pop(T& result)
			{
				std::unique_lock<std::mutex> lock(mMutex);

				if (mQueue.empty())
				{
					return false;
				}

				result = mQueue.front();
				mQueue.pop();

				return true;
			}

			/// <summary>
			/// Pop item from queue
			/// </summary>
			T pop()
			{
				std::unique_lock<std::mutex> lock(mMutex);

				while (mQueue.empty())
				{
					mCondition.wait(lock);
				}

				T result = mQueue.front();
				mQueue.pop();

				return result;
			}
		};

	public:
		/// <summary>
		/// Stop event structure
		/// </summary>
		struct StopEvent
		{
		};

		/// <summary>
		/// Constructor for scheduler
		/// </summary>
		/// <param name="numThreads">How many threads should be used by scheduler for parallel tasks</param>
		Scheduler(unsigned int numThreads = ENGINE_THREADS);

		/// <summary>
		/// Destructor
		/// </summary>
		~Scheduler();

		/// <summary>
		/// Add new task into the scheduler
		/// </summary>
		/// <param name="task">Pointer to task to add</param>
		void Add(Task* task);

		/// <summary>
		/// Start scheduling (call once at the initialization of game engine)
		/// </summary>
		void Start();

		/// <summary>
		/// Stop scheduling (call once at point where game engine is shutdown)
		/// </summary>
		void Stop();

		/// <summary>
		/// Handle stop event
		/// </summary>
		void Handle(const StopEvent&);

		/// <summary>
		/// Handle task completed event
		/// </summary>
		void Handle(const Task::TaskCompleted& tc);

		/// <summary>
		/// Execute single task
		/// </summary>
		void Execute(Task* task);

		/// <summary>
		/// Synchronize the scheduler
		/// </summary>
		void Synchronize();

	private:
		std::thread* mThreads;						// Thread list
		unsigned int mNumThreads;					// Number of threads

		bool mRunning;								// Is scheduler running

		concurrent_queue<Task*> mTaskList[2];		// Task lists (double buffered)
		concurrent_queue<Task*> mBackgroundTasks;	// Background tasks
		concurrent_queue<Task*> mSyncTasks;			// Synchronized tasks

		unsigned int mReadList;						// Read list and write list (helper variables for double buffering the tasks
		unsigned int mWriteList;

		mutable std::mutex mSyncMutex;				// Synchronization task mutex
		std::condition_variable mCondition;			// Condition variable for signaling completed tasks
		size_t mNumTasksToWaitFor;					// How many tasks we need to wait for

		friend int Scheduler_Worker(void* arg);

	public:
		ALIGNED_NEW_DELETE("Engine::Core::Scheduler")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
