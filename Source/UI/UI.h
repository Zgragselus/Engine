#ifndef __UI__H__
#define __UI__H__

#include "Core/Event/EventChannel.h"
#include "Input/Input.h"
#include "Widget.h"
#include <vector>

namespace Engine
{
	class UI
	{
	private:
		/// <summary>Holds all the widgets in this UI container</summary>
		std::vector<Widget*> mWidgets;

	public:
		/// <summary>
		/// Default constructor takes care of registering all the events for input
		/// </summary>
		UI()
		{
			// Register all input events for this class
			static EventChannel chan;
			chan.Add<Keyboard::KeyPressed>(*this);
			chan.Add<Keyboard::KeyReleased>(*this);
			chan.Add<Mouse::ButtonPressed>(*this);
			chan.Add<Mouse::ButtonReleased>(*this);
			chan.Add<Mouse::MouseMove>(*this);
		}

		/// <summary>
		/// Default destructor unregisters all events for input
		/// </summary>
		~UI()
		{
			// Unregister all input events for this class
			static EventChannel chan;
			chan.Remove<Keyboard::KeyPressed>(*this);
			chan.Remove<Keyboard::KeyReleased>(*this);
			chan.Remove<Mouse::ButtonPressed>(*this);
			chan.Remove<Mouse::ButtonReleased>(*this);
			chan.Remove<Mouse::MouseMove>(*this);
		}

		/// <summary>
		/// Key press handler - propagates key press to all widgets in the UI
		/// </summary>
		void Handle(const Keyboard::KeyPressed& kp)
		{
			for (Widget* w : mWidgets)
			{
				w->ProcessKeyPress(kp.mKey);
			}
		}

		/// <summary>
		/// Key release handler - propagates key press to all widgets in the UI
		/// </summary>
		void Handle(const Keyboard::KeyReleased& kp)
		{
			for (Widget* w : mWidgets)
			{
				w->ProcessKeyRelease(kp.mKey);
			}
		}

		/// <summary>
		/// Mouse button press handler - propagates event to all widgets in the UI
		/// </summary>
		void Handle(const Mouse::ButtonPressed& kp)
		{
			for (Widget* w : mWidgets)
			{
				w->ProcessButtonPress(kp.mButton);
			}
		}

		/// <summary>
		/// Mouse button release handler - propagates event to all widgets in the UI
		/// </summary>
		void Handle(const Mouse::ButtonReleased& kp)
		{
			for (Widget* w : mWidgets)
			{
				w->ProcessButtonRelease(kp.mButton);
			}
		}

		/// <summary>
		/// Mouse move handler - propagates event to all widgets in the UI
		/// </summary>
		void Handle(const Mouse::MouseMove& kp)
		{
			for (Widget* w : mWidgets)
			{
				w->ProcessMouseMove(kp.mX, kp.mY);
			}
		}
	};
}

#endif