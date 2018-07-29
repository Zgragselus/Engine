///////////////////////////////////////////////////////////////////////////////////////////////////
//
// ID.h
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

#ifndef __ID__H__
#define __ID__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Memory/Memory.h"
#include <queue>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	///	A class for getting IDs in sequence, in case some are freed, they are stored in a queue so
	/// they can be re-used later
	/// </summary>
	class ID
	{
	private:
		/// <summary>Pointer to log class</summary>
		unsigned int mNext;

		/// <summary>Pointer to log class</summary>
		std::queue<unsigned int> mFree;

	public:
		/// <summary>Set log output</summary>
		/// <param name="l">Pointer to log class</param>
		ID(unsigned int base = 1)
		{
			mNext = base;
		}

		/// <summary>Set log output</summary>
		inline unsigned int Next()
		{
			unsigned int next = 0;

			if (mFree.empty())
			{
				next = mNext;
				mNext++;
			}
			else
			{
				next = mFree.front();
				mFree.pop();
			}

			return next;
		}

		/// <summary>Set log output</summary>
		/// <param name="l">Pointer to log class</param>
		inline void Remove(unsigned int id)
		{
			mFree.emplace(id);
		}

		ALIGNED_NEW_DELETE("Engine::ID")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif