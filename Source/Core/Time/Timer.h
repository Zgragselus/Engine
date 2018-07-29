///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Timer.h
//
// High performance timer
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TIMER_H__
#define __TIMER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/DllDef.h"
#include <chrono>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Using std chrono, allows for high performance timing
	/// </summary>
	class Timer
	{
	private:
		std::chrono::high_resolution_clock::time_point mClock;

	public:
		/// <summary>Constructor, resets timer.</summary>
		Timer()
		{
			Reset();
		}

		/// <summary>Resets timer to 0.</summary>
		void Reset()
		{
			mClock = std::chrono::high_resolution_clock::now();
		}

		/// <summary>Gets time elapsed since last reset.</summary>
		long long int Elapsed()
		{
			std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>
				(std::chrono::high_resolution_clock::now() - mClock);
			long long int time = ms.count();
			return time;
		}
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
