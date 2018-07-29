///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Keyboard.h
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

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#ifdef WIN32
#include <Windows.h>
#endif
#include "Core/Event/EventChannel.h"
#include "Core/Log/Log.h"
#include <cassert>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
#define KEYS_COUNT 65536

	/// <summary>
	/// Keyboard class definition, generalizes keyboard over different systems
	/// </summary>
	class Keyboard
	{
	private:
		/// <summary>
		/// Gets name of key
		/// </summary>
		/// <param name="key">Key ID</param>
		std::string GetKeyName(int key);

		bool mKeyState[KEYS_COUNT];

	public:
		/// <summary>
		/// KeyPressed event structure
		/// </summary>
		struct KeyPressed
		{
			int mKey;

			KeyPressed(int key) :
				mKey(key)
			{
			}
		};

		/// <summary>
		/// KeyReleased event structure
		/// </summary>
		struct KeyReleased
		{
			int mKey;

			KeyReleased(int key) :
				mKey(key)
			{
			}
		};

		/// <summary>
		/// Character added from keyboard
		/// </summary>
		struct KeyChar
		{
			int mKey;

			KeyChar(int key) :
				mKey(key)
			{
			}
		};

		/// <summary>
		/// Constructor
		/// </summary>
		Keyboard()
		{
			EventChannel chan;
			chan.Add<KeyPressed>(*this);
			chan.Add<KeyReleased>(*this);
		}

		/// <summary>
		/// Callback from window
		/// </summary>
		/// <param name="key">Key ID</param>
		/// <param name="mode">Pressed or released?</param>
		static void Callback(int key, int mode)
		{
			static EventChannel chan;

			switch (mode)
			{
			case 0:
				chan.Broadcast(KeyPressed(key));
				break;

			case 1:
				chan.Broadcast(KeyReleased(key));
				break;

			case 2:
				chan.Broadcast(KeyChar(key));
				break;

			default:
				break;
			}
		}

		/// <summary>
		/// Handler for key press event
		/// </summary>
		/// <param name="kp">KeyPressed event structure</param>
		void Handle(const KeyPressed& kp)
		{
			assert(kp.mKey < KEYS_COUNT);
			mKeyState[kp.mKey] = true;
		}

		/// <summary>
		/// Handler for key release event
		/// </summary>
		/// <param name="kr">KeyReleased event structure</param>
		void Handle(const KeyReleased& kr)
		{
			assert(kr.mKey < KEYS_COUNT);
			mKeyState[kr.mKey] = false;
		}

		/// <summary>
		/// Gets state of key
		/// </summary>
		/// <param name="key">Key ID</param>
		bool GetKeyState(int key)
		{
			return mKeyState[key];
		}

		enum
		{
#ifdef WIN32
			KEY_ESCAPE = VK_ESCAPE,
			KEY_HOME = VK_HOME,
			KEY_LEFT_ARROW = VK_LEFT,
			KEY_UP_ARROW = VK_UP,
			KEY_RIGHT_ARROW = VK_RIGHT,
			KEY_DOWN_ARROW = VK_DOWN,
			KEY_PAGE_UP = VK_PRIOR,
			KEY_PAGE_DOWN = VK_NEXT,
			KEY_END = VK_END,
			KEY_INSERT = VK_INSERT,
			KEY_DELETE = VK_DELETE,
			KEY_F1 = VK_F1,
			KEY_F2 = VK_F2,
			KEY_F3 = VK_F3,
			KEY_F4 = VK_F4,
			KEY_F5 = VK_F5,
			KEY_F6 = VK_F6,
			KEY_F7 = VK_F7,
			KEY_F8 = VK_F8,
			KEY_F9 = VK_F9,
			KEY_F10 = VK_F10,
			KEY_F11 = VK_F11,
			KEY_F12 = VK_F12,
			KEY_BACKSPACE = VK_BACK,
			KEY_TAB = VK_TAB,
			KEY_ENTER = VK_RETURN,
			KEY_RETURN = VK_RETURN,
			KEY_SPACE = VK_SPACE,

			KEY_SHIFT_L = VK_LSHIFT,
			KEY_SHIFT_R = VK_RSHIFT,
			KEY_CONTROL_L = VK_LCONTROL,
			KEY_CONTROL_R = VK_RCONTROL,
			KEY_ALT_L = VK_LMENU,
			KEY_ALT_R = VK_RMENU,

			KEY_KP_SEPARATOR = VK_SEPARATOR,
			KEY_KP_0 = VK_NUMPAD0,
			KEY_KP_1 = VK_NUMPAD1,
			KEY_KP_2 = VK_NUMPAD2,
			KEY_KP_3 = VK_NUMPAD3,
			KEY_KP_4 = VK_NUMPAD4,
			KEY_KP_5 = VK_NUMPAD5,
			KEY_KP_6 = VK_NUMPAD6,
			KEY_KP_7 = VK_NUMPAD7,
			KEY_KP_8 = VK_NUMPAD8,
			KEY_KP_9 = VK_NUMPAD9,
			KEY_KP_RETURN = VK_RETURN,
			KEY_KP_ADD = VK_ADD,
			KEY_KP_SUBTRACT = VK_SUBTRACT,
			KEY_KP_MULTIPLY = VK_MULTIPLY,
			KEY_KP_DIVIDE = VK_DIVIDE,

			KEY_NUMLOCK = VK_NUMLOCK,
			KEY_CAPSLOCK = VK_CAPITAL,
			KEY_SCROLLOCK = VK_SCROLL,

			KEY_SUPER_L = VK_LWIN,
			KEY_SUPER_R = VK_RWIN,
			KEY_PAUSE = VK_PAUSE
#else
			KEY_ESCAPE = 0xFF1B,
			KEY_HOME = 0xFF50,
			KEY_LEFT_ARROW = 0xFF51,
			KEY_UP_ARROW = 0xFF52,
			KEY_RIGHT_ARROW = 0xFF53,
			KEY_DOWN_ARROW = 0xFF54,
			KEY_PAGE_UP = 0xFF55,
			KEY_PAGE_DOWN = 0xFF56,
			KEY_END = 0xFF57,
			KEY_INSERT = 0xFF63,
			KEY_DELETE = 0xFFFF,
			KEY_F1 = 0xFFBE,
			KEY_F2 = 0xFFBF,
			KEY_F3 = 0xFFC0,
			KEY_F4 = 0xFFC1,
			KEY_F5 = 0xFFC2,
			KEY_F6 = 0xFFC3,
			KEY_F7 = 0xFFC4,
			KEY_F8 = 0xFFC5,
			KEY_F9 = 0xFFC6,
			KEY_F10 = 0xFFC7,
			KEY_F11 = 0xFFC8,
			KEY_F12 = 0xFFC9,
			KEY_BACKSPACE = 0xFF08,
			KEY_TAB = 0xFF09,
			KEY_ENTER = 0xFF0D,
			KEY_RETURN = 0xFF0D,

			KEY_SHIFT_L = 0xFFE1,
			KEY_SHIFT_R = 0xFFE2,
			KEY_CONTROL_L = 0xFFE3,
			KEY_CONTROL_R = 0xFFE4,
			KEY_ALT_L = 0xFFE9,
			KEY_ALT_R = 0xFFEA,

			KEY_KP_SEPARATOR = 0xFF9F,
			KEY_KP_0 = 0xFF9E,
			KEY_KP_1 = 0xFF9C,
			KEY_KP_2 = 0xFF99,
			KEY_KP_3 = 0xFF9B,
			KEY_KP_4 = 0xFF96,
			KEY_KP_5 = 0xFF9D,
			KEY_KP_6 = 0xFF98,
			KEY_KP_7 = 0xFF95,
			KEY_KP_8 = 0xFF97,
			KEY_KP_9 = 0xFF9A,
			KEY_KP_RETURN = 0xFF8D,
			KEY_KP_ADD = 0xFFAB,
			KEY_KP_SUBTRACT = 0xFFAD,
			KEY_KP_MULTIPLY = 0xFFAA,
			KEY_KP_DIVIDE = 0xFFAF,

			KEY_NUMLOCK = 65407,
			KEY_CAPSLOCK = 65509,
			KEY_SCROLLOCK = 65300,

			KEY_SUPER_L = 65515,
			KEY_SUPER_R = 65383,
			KEY_PAUSE = 65299
#endif
		};

		ALIGNED_NEW_DELETE("Engine::Input::Keyboard")
	};
}

#endif
