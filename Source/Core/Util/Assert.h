///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Assert.h
//
// Engine class that basically works the same way as assert.h or cassert, but allows for logging 
// assertion failures with debug-symbol free executables
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ASSERT__H__
#define __ASSERT__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#define ENABLE_ASSERT

#include "Core/Memory/Memory.h"
#include "Core/Log/Log.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	///	Assertion class - allows for runtime testing of expressions
	/// </summary>
	class Assert
	{
	private:
		/// <summary>Log where the assertion fails are written into</summary>
		static Log* mLog;

	public:
		/// <summary>
		///	Set output log
		/// </summary>
		/// <param name="log">Pointer to output log</param>
		static void SetLog(Log* log)
		{
			mLog = log;
		}

		/// <summary>
		///	Assertion test
		/// </summary>
		/// <param name="expression">Expression to check</param>
		/// <param name="message">Message string written into log in case of failure</param>
		static void Test(bool expression, const std::string& message)
		{
#ifdef ENABLE_ASSERT
			if (!expression && mLog)
			{
				mLog->Print("Assertion failed!", message);
			}
#endif
		}

		ALIGNED_NEW_DELETE("Engine::Assert")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif