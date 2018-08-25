///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Time.h
//
// Following file contains class implementing time and date
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TIME_H__
#define __TIME_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/DllDef.h"
#include <string>
#include "Timer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Time class, allows for getting date and time in common format.
	/// </summary>
	class Time
	{
	public:
		/// <summary>Returns date and time inside string.</summary>
		static const std::string GetTime();
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
	