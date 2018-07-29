///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Core.h
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

#ifndef __CORE__H__
#define __CORE__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Log/Log.h"
#include "Core/Constants/Constants.h"
#include "Core/System/System.h"
#include "Core/Scheduler/Scheduler.h"
#include "Core/Memory/Memory.h"
#include <map>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	///	
	/// </summary>
	class Core
	{
	private:
		static Core* mInstance;			// Single global instance of Core

		/// <summary>Pointer to log class</summary>
		Log* mLog;

		/// <summary>Pointer to constants class holding options</summary>
		Constants* mOptions;
		
		/// <summary>
		/// Constructor
		/// </summary>
		Core() 
		{
			mLog = NULL;
		}

		/// <summary>
		/// Private copy constructor
		/// </summary>
		Core(const Core&) {}

		/// <summary>
		/// Destructor
		/// </summary>
		~Core() {}

		/// <summary>
		/// Assignment operator is also private
		/// </summary>
		Core& operator=(const Core&) { return *this;  }

	public:
		/// <summary>Set log output</summary>
		/// <param name="l">Pointer to log class</param>
		void SetLog(Log* l)
		{
			mLog = l;
		}

		/// <summary>Set log output</summary>
		/// <param name="l">Pointer to log class</param>
		void SetOptions(Constants* c)
		{
			mOptions = c;
		}

		/// <summary>Set log output</summary>
		/// <param name="l">Pointer to log class</param>
		Log* GetLog()
		{
			return mLog;
		}

		/// <summary>Set log output</summary>
		/// <param name="l">Pointer to log class</param>
		Constants* GetOptions()
		{
			return mOptions;
		}

		/// <summary>
		/// Run the core
		/// </summary>
		void Run()
		{
			InitializeSystems();
			mScheduler.Start();
			ShutdownSystems();
		}

		/// <summary>
		/// Stop scheduler
		/// </summary>
		void Stop()
		{
			mChannel.Broadcast(Scheduler::StopEvent());
		}

		/// <summary>
		/// Add system into engine
		/// </summary>
		/// <param name="s">Add system</param>
		void Add(System* s)
		{
			if (mSystems.find(s->GetName()) == mSystems.end())
			{
				if (s->mSystemUpdater != NULL)
				{
					mScheduler.Add(s->mSystemUpdater);
				}

				mSystemsOrder.push_back(s);
				mSystems.insert(std::pair<std::string, System*>(s->GetName(), s));
			}
			else
			{
				if (mLog)
				{
					mLog->Print("Core", std::string("System ") + s->GetName() + std::string(" has been already added!"));
				}
			}
		}

		/// <summary>
		/// Get system by name
		/// </summary>
		/// <param name="name">Name of the system</param>
		System* Get(const std::string& name)
		{
			std::map<std::string, System*>::const_iterator it = mSystems.find(name);

			if (it == mSystems.end())
			{
				if (mLog)
				{
					mLog->Print("Core", std::string("Cannot find system ") + it->second->GetName());
				}
				return NULL;
			}

			return it->second;
		}

		/// <summary>
		/// Get Instance of Core
		/// </summary>
		static Core* Instance();

		/// <summary>
		/// Dispose of the instance
		/// </summary>
		static void Dispose();

	private:
		/// <summary>
		/// Initialize all the systems in order in which they were added
		/// </summary>
		void InitializeSystems()
		{
			for (std::vector<System*>::iterator it = mSystemsOrder.begin();
				it != mSystemsOrder.end(); 
				it++)
			{
				(*it)->Init();
				if (mLog)
				{
					mLog->Print("Core", std::string("Initializing system ") + (*it)->GetName());
				}
			}
		}

		/// <summary>
		/// Shut down all the systems in reverse order compared to initialization
		/// </summary>
		void ShutdownSystems()
		{
			for (std::vector<System*>::reverse_iterator it = mSystemsOrder.rbegin();
				it != mSystemsOrder.rend();
				it++)
			{
				(*it)->Shutdown();
				if (mLog)
				{
					mLog->Print("Core", std::string("Shutting down system ") + (*it)->GetName());
				}
			}
		}

		std::map<std::string, System*> mSystems;	// Container for all the systems
		std::vector<System*> mSystemsOrder;			// Vector of all systems, ordered so they are initialized in proper order
		Scheduler mScheduler;						// Scheduler for engine
		EventChannel mChannel;						// Event channel

	public:
		ALIGNED_NEW_DELETE("Engine::Core")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
