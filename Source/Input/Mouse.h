///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Mouse.h
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

#ifndef __MOUSE_H__
#define __MOUSE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Event/EventChannel.h"
#include "Core/System/System.h"
#include "Core/Log/Log.h"
#include <cassert>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
#define MOUSE_BUTTONS_COUNT 16

	/// <summary>
	/// Mouse class definition, generalizes mouse over different systems
	/// </summary>
	class Mouse
	{
	private:
		/// <summary>
		/// Gets name of button
		/// </summary>
		/// <param name="button">Button ID</param>
		std::string GetButtonName(int button);

		bool mButtons[MOUSE_BUTTONS_COUNT];		// Buttons state array
		int mX, mY;								// Mouse position
		int mDelta;								// Mouse wheel last delta

	public:
		/// <summary>
		/// ButtonPressed message structure
		/// </summary>
		struct ButtonPressed
		{
			ButtonPressed(int button) :
			mButton(button)
			{
			}

			int mButton;
		};

		/// <summary>
		/// ButtonReleased message structure
		/// </summary>
		struct ButtonReleased
		{
			ButtonReleased(int button) :
			mButton(button)
			{
			}

			int mButton;
		};

		/// <summary>
		/// MouseMove message structure
		/// </summary>
		struct MouseMove
		{
			MouseMove(int x, int y) :
			mX(x), mY(y)
			{
			}

			int mX, mY;
		};

		/// <summary>
		/// MouseWheel message structure
		/// </summary>
		struct MouseWheel
		{
			MouseWheel(int delta) : mDelta(delta)
			{
			}

			int mDelta;
		};

		/// <summary>
		/// Constructor
		/// </summary>
		Mouse()
		{
			for (unsigned int i = 0; i < MOUSE_BUTTONS_COUNT; i++)
			{
				mButtons[i] = false;
			}

			EventChannel chan;
			chan.Add<ButtonPressed>(*this);
			chan.Add<ButtonReleased>(*this);
			chan.Add<MouseMove>(*this);
			chan.Add<MouseWheel>(*this);
		}

		/// <summary>
		/// Depending on mode either sets button to pressed or released or refreshes mouse positSion
		/// </summary>
		/// <param name="button">Button ID</param>
		/// <param name="x">New X position</param>
		/// <param name="y">New Y position</param>
		/// <param name="mode">Which state is being changed</param>
		static void Callback(int button, int x, int y, int mode)
		{
			static EventChannel chan;

			switch (mode)
			{
			case 0:
				chan.Broadcast(ButtonPressed(button));
				break;

			case 1:
				chan.Broadcast(ButtonReleased(button));
				break;

			case 2:
				chan.Broadcast(MouseMove(x, y));
				break;

			case 3:
				chan.Broadcast(MouseWheel(button));
				break;

			default:
				break;
			}
		}

		/// <summary>
		/// Handler for ButtonPressed event
		/// </summary>
		/// <param name="bp">Message structure</param>
		void Handle(const ButtonPressed& bp)
		{
			assert(bp.mButton < MOUSE_BUTTONS_COUNT);
			mButtons[bp.mButton] = true;
		}

		/// <summary>
		/// Handler for ButtonReleased event
		/// </summary>
		/// <param name="br">Message structure</param>
		void Handle(const ButtonReleased& br)
		{
			assert(br.mButton < MOUSE_BUTTONS_COUNT);
			mButtons[br.mButton] = false;
		}

		/// <summary>
		/// Handler for mousemove event
		/// </summary>
		/// <param name="mm">Data for mouse move</param>
		void Handle(const MouseMove& mm)
		{
			mX = mm.mX;
			mY = mm.mY;
		}

		/// <summary>
		/// Handler for mousewheel event
		/// </summary>
		/// <param name="mw">Data for mouse wheel</param>
		void Handle(const MouseWheel& mw)
		{
			mDelta = mw.mDelta;
		}

		/// <summary>
		/// Gets button state
		/// </summary>
		/// <param name="button">Button ID</param>
		bool GetButtonState(int button)
		{
			return mButtons[button];
		}

		/// <summary>
		/// Gets position
		/// </summary>
		/// <param name="x">Ref to X coordinate</param>
		/// <param name="y">Ref to Y coordinate</param>
		void GetPosition(int& x, int& y)
		{
			x = mX;
			y = mY;
		}

		ALIGNED_NEW_DELETE("Engine::Input::Mouse")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
