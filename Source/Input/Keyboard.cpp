///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Keyboard.cpp
//
// Following file contains Keyboard input definition
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

#include "Keyboard.h"
#include "Core/Math/Numeric/Common.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Declaration section

using namespace Engine;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

/// <summary>
/// Gets name of key
/// </summary>
/// <param name="key">Key ID</param>
std::string Keyboard::GetKeyName(int key)
{
	switch (key)
	{
	case Keyboard::KEY_ESCAPE:
		return "KEY_ESCAPE";
	case Keyboard::KEY_HOME:
		return "KEY_HOME";
	case Keyboard::KEY_LEFT_ARROW:
		return "KEY_LEFT_ARROW";
	case Keyboard::KEY_UP_ARROW:
		return "KEY_UP_ARROW";
	case Keyboard::KEY_RIGHT_ARROW:
		return "KEY_RIGHT_ARROW";
	case Keyboard::KEY_DOWN_ARROW:
		return "KEY_DOWN_ARROW";
	case Keyboard::KEY_PAGE_UP:
		return "KEY_PAGE_UP";
	case Keyboard::KEY_PAGE_DOWN:
		return "KEY_PAGE_DOWN";
	case Keyboard::KEY_END:
		return "KEY_END";
	case Keyboard::KEY_INSERT:
		return "KEY_INSERT";
	case Keyboard::KEY_DELETE:
		return "KEY_DELETE";
	case Keyboard::KEY_F1:
		return "KEY_F1";
	case Keyboard::KEY_F2:
		return "KEY_F2";
	case Keyboard::KEY_F3:
		return "KEY_F3";
	case Keyboard::KEY_F4:
		return "KEY_F4";
	case Keyboard::KEY_F5:
		return "KEY_F5";
	case Keyboard::KEY_F6:
		return "KEY_F6";
	case Keyboard::KEY_F7:
		return "KEY_F7";
	case Keyboard::KEY_F8:
		return "KEY_F8";
	case Keyboard::KEY_F9:
		return "KEY_F9";
	case Keyboard::KEY_F10:
		return "KEY_F10";
	case Keyboard::KEY_F11:
		return "KEY_F11";
	case Keyboard::KEY_F12:
		return "KEY_F12";
	case Keyboard::KEY_BACKSPACE:
		return "KEY_BACKSPACE";
	case Keyboard::KEY_TAB:
		return "KEY_TAB";
	case Keyboard::KEY_RETURN:
		return "KEY_RETURN";
	case Keyboard::KEY_SHIFT_L:
		return "KEY_SHIFT_L";
	case Keyboard::KEY_SHIFT_R:
		return "KEY_SHIFT_R";
	case Keyboard::KEY_CONTROL_L:
		return "KEY_CONTROL_L";
	case Keyboard::KEY_CONTROL_R:
		return "KEY_CONTROL_R";
	case Keyboard::KEY_ALT_L:
		return "KEY_ALT_L";
	case Keyboard::KEY_ALT_R:
		return "KEY_ALT_R";
	case Keyboard::KEY_KP_SEPARATOR:
		return "KEY_KP_SEPARATOR";
	case Keyboard::KEY_KP_0:
		return "0 (Keypad)";
	case Keyboard::KEY_KP_1:
		return "1 (Keypad)";
	case Keyboard::KEY_KP_2:
		return "2 (Keypad)";
	case Keyboard::KEY_KP_3:
		return "3 (Keypad)";
	case Keyboard::KEY_KP_4:
		return "4 (Keypad)";
	case Keyboard::KEY_KP_5:
		return "5 (Keypad)";
	case Keyboard::KEY_KP_6:
		return "6 (Keypad)";
	case Keyboard::KEY_KP_7:
		return "7 (Keypad)";
	case Keyboard::KEY_KP_8:
		return "8 (Keypad)";
	case Keyboard::KEY_KP_9:
		return "9 (Keypad)";
#ifndef WIN32
	case Keyboard::KEY_KP_RETURN:
		return "KEY_KP_RETURN";
#endif
	case Keyboard::KEY_KP_ADD:
		return "KEY_KP_ADD";
	case Keyboard::KEY_KP_SUBTRACT:
		return "KEY_KP_SUBTRACT";
	case Keyboard::KEY_KP_MULTIPLY:
		return "KEY_KP_MULTIPLY";
	case Keyboard::KEY_KP_DIVIDE:
		return "KEY_KP_DIVIDE";
	case Keyboard::KEY_NUMLOCK:
		return "KEY_NUMLOCK";
	case Keyboard::KEY_CAPSLOCK:
		return "KEY_CAPSLOCK";
	case Keyboard::KEY_SCROLLOCK:
		return "KEY_SCROLLOCK";
	case Keyboard::KEY_SUPER_L:
		return "KEY_SUPER_L";
	case Keyboard::KEY_SUPER_R:
		return "KEY_SUPER_R";
	case Keyboard::KEY_PAUSE:
		return "KEY_PAUSE";
	default:
		return std::string(1, (char)key);
		break;
	}
}
