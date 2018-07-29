///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Window.h
//
// Following file contains Window class definition
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WINDOW_H__
#define __WINDOW_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include <Windows.h>
#include "Core/Constants/Constants.h"
#include "Core/Log/Log.h"
#include "Core/Event/EventChannel.h"
#include "Core/Scheduler/Scheduler.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

#ifdef _WIN32
//#include <gl/GL.h>
//#include "GL/glext.h"
//#include "GL/wglext.h"
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	class Window
	{
	public:
		struct Resize
		{
			Resize(int width, int height) :
				mWidth(width), mHeight(height)
			{
			}

			int mWidth, mHeight;
		};

		Window(Log* log, Constants* options);
		~Window();

		void Create();
		void Destroy();

		void Paint();

		void Process();

		void GetSize(int* width, int* height);
		void GetPosition(int* x, int* y);

		void GetCursorPosition(int* x, int* y);
		void SetCursorPosition(int x, int y);

		HDC GetDeviceContext()
		{
			return mDeviceContext;
		}

		HWND GetWindow()
		{
			return mHwnd;
		}

	private:
		Log* mLog;
		Constants* mOptions;

#ifdef _WIN32
		HWND mHwnd;
		HINSTANCE mInstance;
		HDC mDeviceContext;

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l);
		LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l);
#else
		Display* mDisplay;
		GLXContext mContext;
		Colormap mColorMap;
		Window mWindow;
#endif

		int mWidth;
		int mHeight;

		EventChannel mChannel;

	public:
		bool mSwappable;

		ALIGNED_NEW_DELETE("Engine::Window")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
