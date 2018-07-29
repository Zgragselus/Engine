///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Profiler.h
//
// The class that allows to setup various channels to profile, summing up time on single app
// loop.
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PROFILER_H__
#define __PROFILER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/DllDef.h"
#include "../Time/Timer.h"
#include <map>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Using std chrono, allows for high performance timing
	/// </summary>
	class Profiler
	{
	private:	
		std::map<std::string, unsigned int> mIDs;	// Maps string names to IDs of profiling information
		std::vector<Timer*> mTimers;				// One timer per profiling channel
		std::vector<long long int> mAccumulators;	// Accumulate time spent executing parts of the code
		std::vector<unsigned int> mCalls;			// Counts the number the function has been called
		unsigned int mNextID;

	public:
		/// <summary>Constructor, resets timer.</summary>
		Profiler()
		{
			mNextID = 0;
		}

		/// <summary>Destructor, clear used memory</summary>
		~Profiler()
		{
			for (auto t : mTimers)
			{
				delete t;
			}
			mTimers.clear();
		}

		/// <summary>Add new profile channel</summary>
		/// <param name="name">Channel name</param>
		void Add(const std::string& name)
		{
			mIDs[name] = mNextID;
			mNextID++;

			mTimers.push_back(new Timer());
			mAccumulators.push_back(0);
			mCalls.push_back(0);
		}

		/// <summary>Reset given profile channel</summary>
		/// <param name="name">Channel name</param>
		void Reset(const std::string& name)
		{
			auto it = mIDs.find(name);
			if (it != mIDs.end())
			{
				unsigned int id = it->second;
				mAccumulators[id] = 0;
				mCalls[id] = 0;
			}
		}

		/// <summary>Begin profiling execution time on given channel</summary>
		/// <param name="name">Channel name</param>
		void BeginProfile(const std::string& name)
		{
			auto it = mIDs.find(name);
			if (it != mIDs.end())
			{
				unsigned int id = it->second;
				mTimers[id]->Reset();
			}
		}

		/// <summary>Finish profiling execution time on given channel</summary>
		/// <param name="name">Channel name</param>
		void EndProfile(const std::string& name)
		{
			auto it = mIDs.find(name);
			if (it != mIDs.end())
			{
				unsigned int id = it->second;
				long long elapsed = mTimers[id]->Elapsed();
				mAccumulators[id] += elapsed;
				mCalls[id]++;
			}
		}
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
