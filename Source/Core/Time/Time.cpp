///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Time.cpp
//
// Following file implements methods defined in Time.h.
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

#include "Time.h"
#include <sstream>
#include <ctime>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Declaration section

using namespace Engine;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

/// <summary>Returns date and time inside string.</summary>
const std::string Time::GetTime()
{
	// Get current date and time into C-like string
	char buffer[80] = { 0 };
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%d-%m-%Y %I:%M:%S", timeinfo);

	return std::string(buffer);
}