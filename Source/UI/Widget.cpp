#include "Widget.h"

using namespace Engine;

static void HandleMouseMove(Widget* w, int x, int y)
{
	bool intersect = w->Intersect(int2(x, y));

	if (intersect && !w->IsHovering())
	{
		w->SetHover(true);

		for (Widget::EnterEvent e : w->mEnterEvents)
		{
			e();
		}
	}
	else if (!intersect && w->IsHovering())
	{
		w->SetHover(false);

		for (Widget::ExitEvent e : w->mExitEvents)
		{
			e();
		}
	}

	for (Widget::MotionEvent e : w->mMotionEvents)
	{
		e(x, y);
	}
}

static void HandleButtonPress(Widget* w, int button)
{
	if (w->IsHovering())
	{
		for (Widget::ButtonPressEvent e : w->mButtonPressEvents)
		{
			e(button);
		}
	}
}

static void HandleButtonRelease(Widget* w, int button)
{
	if (w->IsActive())
	{
		for (Widget::ButtonReleaseEvent e : w->mButtonReleaseEvents)
		{
			e(button);
		}
	}
}

static void HandleKeyPress(Widget* w, int key)
{
	for (Widget::KeyPressEvent e : w->mKeyPressEvents)
	{
		e(key);
	}
}

static void HandleKeyRelease(Widget* w, int key)
{
	for (Widget::KeyReleaseEvent e : w->mKeyReleaseEvents)
	{
		e(key);
	}
}

Widget::Widget(const std::string& name, Widget* parent, const int2& position, const int2& size, const int anchor, const int alignment, const bool visible) :
	mName(name),
	mParent(parent),
	mPosition(position),
	mSize(size),
	mAnchor(anchor),
	mAlignment(alignment),
	mVisible(visible),
	mHover(false)
{
	mMouseMoveSignals.push_back(HandleMouseMove);
	mButtonPressSignals.push_back(HandleButtonPress);
	mButtonReleaseSignals.push_back(HandleButtonRelease);
	mKeyPressSignals.push_back(HandleKeyPress);
	mKeyReleaseSignals.push_back(HandleKeyRelease);
}

Widget::~Widget()
{

}

/// <summary>
/// Add event
/// </summary>
void Widget::AddEvent(const std::string& funcname, Event type)
{
	// Based on type add event to an array
	switch (type)
	{
	case ENTER:
	{
		EnterEvent fn = g_mFunctions->Get<EnterEvent>(funcname);
		if (fn)
		{
			mEnterEvents.push_back(fn);
		}
	}
	break;

	case EXIT:
	{
		ExitEvent fn = g_mFunctions->Get<ExitEvent>(funcname);
		if (fn)
		{
			mExitEvents.push_back(fn);
		}
	}
	break;

	case KEY_PRESS:
	{
		KeyPressEvent fn = g_mFunctions->Get<KeyPressEvent>(funcname);
		if (fn)
		{
			mKeyPressEvents.push_back(fn);
		}
	}
	break;

	case KEY_RELEASE:
	{
		KeyReleaseEvent fn = g_mFunctions->Get<KeyReleaseEvent>(funcname);
		if (fn)
		{
			mKeyReleaseEvents.push_back(fn);
		}
	}
	break;

	case BUTTON_PRESS:
	{
		ButtonPressEvent fn = g_mFunctions->Get<ButtonPressEvent>(funcname);
		if (fn)
		{
			mButtonPressEvents.push_back(fn);
		}
	}
	break;

	case BUTTON_RELEASE:
	{
		ButtonReleaseEvent fn = g_mFunctions->Get<ButtonReleaseEvent>(funcname);
		if (fn)
		{
			mButtonReleaseEvents.push_back(fn);
		}
	}
	break;

	case MOTION:
	{
		MotionEvent fn = g_mFunctions->Get<MotionEvent>(funcname);
		if (fn)
		{
			mMotionEvents.push_back(fn);
		}
	}
	break;
	}
}