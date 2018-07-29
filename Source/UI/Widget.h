///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Widget.h
//
// Defines the default class for all UI elements. The widget is a top-level class, and each UI
// element derives from it along with full functionality for event handling.
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __UI__WIDGET__H__
#define __UI__WIDGET__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Math/Numeric/Int2.h"
#include "Core/Functions/Functions.h"
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>Forward declaration for UI container</summary>
	class UI;

	/// <summary>
	/// Top-level UI element class
	/// </summary>
	class Widget
	{
	public:
		/// <summary>Anchor enum - allows to anchor given widget to some side of the window</summary>
		enum Side
		{
			LEFT = 0x1,
			RIGHT = 0x2,
			TOP = 0x4,
			BOTTOM = 0x8
		};

		/// <summary>
		/// Different signals that are bind-able by user (typedefs and enumeration of types)
		/// </summary>
		enum Event
		{
			ENTER,
			EXIT,
			KEY_PRESS,
			KEY_RELEASE,
			BUTTON_PRESS,
			BUTTON_RELEASE,
			MOTION
		};

		typedef void(*EnterEvent)();
		typedef void(*ExitEvent)();
		typedef void(*KeyPressEvent)(int);
		typedef void(*KeyReleaseEvent)(int);
		typedef void(*ButtonPressEvent)(int);
		typedef void(*ButtonReleaseEvent)(int);
		typedef void(*MotionEvent)(int, int);

		/// <summary>
		/// Arrays for holding all the events set by user
		/// <summary>
		std::vector<EnterEvent> mEnterEvents;
		std::vector<ExitEvent> mExitEvents;
		std::vector<KeyPressEvent> mKeyPressEvents;
		std::vector<KeyReleaseEvent> mKeyReleaseEvents;
		std::vector<ButtonPressEvent> mButtonPressEvents;
		std::vector<ButtonReleaseEvent> mButtonReleaseEvents;
		std::vector<MotionEvent> mMotionEvents;

		/// <summary>
		/// Adds event function to process when certain event happens
		/// </summary>
		/// <param name="funcname">Name of the functions to invoke (grabbed from global function space in Core/Functions)</param>
		/// <param name="type">Describes which type of event invokes this function</param>
		void AddEvent(const std::string& funcname, Event type);

	protected:
		/// <summary>Parent widget</summary>
		Widget* mParent;

		/// <summary>Holds all the widgets in this widget</summary>
		std::vector<Widget*> mWidgets;

		/// <summary>Position - relative to parent widget (NULL - top-left corner of screen)</summary>
		int2 mPosition;

		/// <summary>Size</summary>
		int2 mSize;

		/// <summary>Anchor - binary OR between side enumeration values</summary>
		int mAnchor;

		/// <summary>Alignment - binary OR between side enumeration values</summary>
		int mAlignment;

		/// <summary>Is widget visible?</summary>
		bool mVisible;

		/// <summary>Is widget hovering?</summary>
		bool mHover;

		/// <summary>Is widget being pressed?</summary>
		bool mActive;

		/// <summary>Widget name</summary>
		std::string mName;

		/// <summary>
		/// Typedefs, containers and methods for signals. A signal is event received from Input propagated
		/// through the widgets and therefore also firing events that do some processing
		/// </summary>
		typedef void(*KeyPressSignal)(Widget*, int);
		typedef void(*KeyReleaseSignal)(Widget*, int);
		typedef void(*ButtonPressSignal)(Widget*, int);
		typedef void(*ButtonReleaseSignal)(Widget*, int);
		typedef void(*MouseMoveSignal)(Widget*, int, int);

		std::vector<KeyPressSignal> mKeyPressSignals;
		std::vector<KeyReleaseSignal> mKeyReleaseSignals;
		std::vector<ButtonPressSignal> mButtonPressSignals;
		std::vector<ButtonReleaseSignal> mButtonReleaseSignals;
		std::vector<MouseMoveSignal> mMouseMoveSignals;

		inline void ProcessKeyPress(int key)
		{
			for (Widget* w : mWidgets)
			{
				w->ProcessKeyPress(key);
			}

			for (KeyPressSignal fn : mKeyPressSignals)
			{
				fn(this, key);
			}
		}

		inline void ProcessKeyRelease(int key)
		{
			for (Widget* w : mWidgets)
			{
				w->ProcessKeyRelease(key);
			}

			for (KeyReleaseSignal fn : mKeyReleaseSignals)
			{
				fn(this, key);
			}
		}

		inline void ProcessButtonPress(int button)
		{
			for (Widget* w : mWidgets)
			{
				w->ProcessButtonPress(button);
			}

			for (ButtonPressSignal fn : mButtonPressSignals)
			{
				fn(this, button);
			}
		}

		inline void ProcessButtonRelease(int button)
		{
			for (Widget* w : mWidgets)
			{
				w->ProcessButtonRelease(button);
			}

			for (ButtonReleaseSignal fn : mButtonReleaseSignals)
			{
				fn(this, button);
			}
		}

		inline void ProcessMouseMove(int x, int y)
		{
			for (Widget* w : mWidgets)
			{
				w->ProcessMouseMove(x, y);
			}

			for (MouseMoveSignal fn : mMouseMoveSignals)
			{
				fn(this, x, y);
			}
		}

	public:
		Widget(const std::string& name, Widget* parent = NULL, const int2& position = int2(), const int2& size = int2(), const int anchor = TOP | LEFT, const int alignment = TOP | LEFT, const bool visible = true);

		~Widget();

		inline const int2& GetPosition()
		{
			return mPosition;
		}

		inline const int2& GetSize()
		{
			return mSize;
		}

		inline const int GetAnchor()
		{
			return mAnchor;
		}

		inline const int GetAlignment()
		{
			return mAlignment;
		}

		inline const bool IsVisible()
		{
			return mVisible;
		}

		inline const bool IsHovering()
		{
			return mHover;
		}

		inline void SetHover(bool hover)
		{
			mHover = hover;
		}

		inline const bool IsActive()
		{
			return mActive;
		}

		inline const std::string& GetName()
		{
			return mName;
		}

		/// <summary>
		/// Calculates base position (top-left) taking alignment into account
		/// </summary>
		inline int2 GetBasePosition()
		{
			int2 base = mPosition;
			int2 halfSize = mSize / 2;

			if (mAlignment & LEFT)
			{
				base.x += halfSize.x;
			}

			if (mAlignment & RIGHT)
			{
				base.x -= halfSize.x;
			}

			if (mAlignment & TOP)
			{
				base.y += halfSize.y;
			}

			if (mAlignment & BOTTOM)
			{
				base.y -= halfSize.y;
			}

			return base;
		}

		/// <summary>
		/// Intersect widget against 2D point
		/// </summary>
		/// <param name="pos">2D position representing point we're intersecting against</param>
		inline bool Intersect(const int2& pos)
		{
			int2 base = GetBasePosition();
			int2 delta = base - pos;

			if (abs(delta.x) < mSize.x && abs(delta.y) < mSize.y)
			{
				return true;
			}

			return false;
		}
		
		friend class UI;
	};
}

#endif