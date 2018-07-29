///////////////////////////////////////////////////////////////////////////////////////////////////
//
// System.h
//
// System class of engine is a super class for any system in an engine.
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/DllDef.h"
#include "Core/Task/Task.h"
#include "Core/Event/EventChannel.h"
#include "Core/Memory/Memory.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// General engine system class
	/// </summary>
	class System
	{
	public:
		/// <summary>
		/// System initialize message structure
		/// </summary>
		struct SystemInitializing
		{
			SystemInitializing(System* s) :
			mSystem(s)
			{
			}

			System* mSystem;		// Pointer to system for this message
		};

		/// <summary>
		/// System shutdown message structure
		/// </summary>
		struct SystemShutdown
		{
			SystemShutdown(System* s) :
			mSystem(s)
			{
			}

			System* mSystem;		// Pointer to system for this message
		};

		/// <summary>
		/// System updater task
		/// </summary>
		struct SystemUpdater : public Task
		{
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="s">System that will be held by the task</param>
			/// <param name="taskFlags">Flags for task (defaults to repeating sync task)</param>
			SystemUpdater(System* s, unsigned int taskFlags = Task::SINGLE_THREADED_REPEATING) :
				Task(taskFlags), mSystem(s)
			{
			}

			/// <summary>
			/// Execute the task
			/// </summary>
			virtual void Run()
			{
				mSystem->Update();
			}

			System* mSystem;		// Pointer to system for this message
		};

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="name">Name of the system</param>
		System(const std::string& name) :
			mName(name)
		{
			mSystemUpdater = NULL;
		}

		/// <summary>
		/// Destructor (destroys system updater, if any)
		/// </summary>
		virtual ~System()
		{
			if (mSystemUpdater)
			{
				delete mSystemUpdater;
			}
		}

		/// <summary>
		/// Initialize the system
		/// </summary>
		virtual bool Init()
		{
			mChannel.Broadcast(SystemInitializing(this));
			return true;
		}

		/// <summary>
		/// Enable updater
		/// </summary>
		/// <param name="taskFlags">Updater task flags</param>
		void EnableUpdater(unsigned int taskFlags = Task::SINGLE_THREADED_REPEATING)
		{
			if (mSystemUpdater)
			{
				delete mSystemUpdater;
			}

			mSystemUpdater = new SystemUpdater(this, taskFlags);
		}

		/// <summary>
		/// Update system
		/// </summary>
		virtual void Update()
		{
		}

		/// <summary>
		/// Shutdown system
		/// </summary>
		virtual void Shutdown()
		{
			mChannel.Broadcast(SystemShutdown(this));
		}

		/// <summary>
		/// Get system name
		/// </summary>
		const std::string& GetName() const
		{
			return mName;
		}

	protected:
		friend class Core;					// Allow Core class to access protected/private members

		EventChannel mChannel;				// Event channel
		SystemUpdater* mSystemUpdater;		// Pointer to system updater
		std::string mName;					// Name of the system

	public:
		ALIGNED_NEW_DELETE("Engine::Core::System")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
