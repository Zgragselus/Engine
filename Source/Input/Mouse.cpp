///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Mouse.cpp
//
// Following file contains Mouse input definition
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

#include "Mouse.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Declaration section

using namespace Engine;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

/// <summary>
/// Gets name of button
/// </summary>
/// <param name="button">Button ID</param>
std::string Mouse::GetButtonName(int button)
{
	switch (button)
	{
	case 0:
		return "LEFT_MOUSE_BUTTON";
	case 1:
		return "MIDDLE_MOUSE_BUTTON";
	case 2:
		return "RIGHT_MOUSE_BUTTON";
	case 3:
		return "MOUSE_WHEEL_UP";
	case 4:
		return "MOUSE_WHEEL_DOWN";
	default:
		return "";
	}
}
