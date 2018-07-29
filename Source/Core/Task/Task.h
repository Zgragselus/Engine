///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Task.h
//
// Following file contains Task basic definition
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TASK__H__
#define __TASK__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/DllDef.h"
#include "Core/Memory/Memory.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>Task class</summary>
	class Task
	{
	public:
		/// <summary>TaskBeginning event</summary>
		struct TaskBeginning
		{
			TaskBeginning(Task* t) :
			mTask(t)
			{
			}

			Task* mTask;
		};

		/// <summary>TaskCompleted event</summary>
		struct TaskCompleted
		{
			TaskCompleted(Task* t) :
			mTask(t)
			{
			}

			Task* mTask;
		};

		/// <summary>Different task types - whether it is async or sync, repeating, etc.</summary>
		enum
		{
			NONE = 0x0,
			REPEATING = 0x1,
			THREAD_SAFE = 0x2,
			FRAME_SYNC = 0x4,

			SINGLE_THREADED = NONE,
			SINGLE_THREADED_REPEATING = REPEATING,
			BACKGROUND = THREAD_SAFE,
			BACKGROUND_REPEATING = THREAD_SAFE | REPEATING,
			BACKGROUND_SYNC = THREAD_SAFE | FRAME_SYNC,
			BACKGROUND_SYNC_REPEATING = THREAD_SAFE | REPEATING | FRAME_SYNC,

			ALL = ~0x0
		};

		/// <summary>Constructor</summary>
		/// <param name="flags">With what flags we want to create the task</param>
		Task(unsigned int flags = SINGLE_THREADED_REPEATING);

		/// <summary>Virtual destructor</summary>
		virtual ~Task();

		/// <summary>Pure virtual function to run task</summary>
		virtual void Run() = 0;

		/// <summary>Get task flags (= task type)</summary>
		unsigned int GetTaskFlags() const
		{
			return mTaskFlags;
		}

	private:
		unsigned int mTaskFlags;		// Task flags

	public:
		ALIGNED_NEW_DELETE("Engine::Core::Task")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
