///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Window.cpp header
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

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#ifdef _WIN32
#include <windowsx.h>
#endif
#include "Window.h"
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Declaration section

using namespace Engine;

#ifdef _WIN32
#else
#define GLX_CONTEXT_MAJOR_VERSION_ARB					0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB					0x2092
#define GLX_CONTEXT_FLAGS_ARB							0x2094
#define GLX_CONTEXT_PROFILE_MASK_ARB					0x9126
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB				0x00000001
#define GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB		0x00000002
typedef GLXContext(*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

static bool IsExtensionSupported(const char* mExtList, const char* mExtension)
{
	const char* mStart;
	const char* mWhere;
	const char* mTerminator;

	mWhere = strchr(mExtension, ' ');
	if (mWhere || mExtension == '\0')
	{
		return false;
	}

	for (mStart = mExtList;;)
	{
		mWhere = strstr(mStart, mExtension);

		if (!mWhere)
		{
			return false;
		}

		mTerminator = mWhere + strlen(mExtension);

		if (mWhere == mStart || *(mWhere - 1) == ' ')
		{
			if (*mTerminator == ' ' || *mTerminator == '\0')
			{
				return true;
			}
		}

		mStart = mTerminator;
	}

	return false;
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

Window::Window(Log* log, Constants* options)
{
	mLog = log;
	mOptions = options;
}

Window::~Window()
{
}

#ifdef _WIN32
LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
	switch (msg)
	{
	case WM_CREATE:
		{
		}
		break;

	default:
		{
#ifdef _WIN64
			Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
#else
			Window* window = (Window*)GetWindowLong(hwnd, GWL_USERDATA);
#endif
			return window->WindowProc(hwnd, msg, w, l);
		}
	}

	return 0;
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
	switch (msg)
	{
	case WM_ACTIVATE:
		return 0;
		break;

	case WM_SYSCOMMAND:
		switch (w)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0;
		}
		break;

	case WM_DESTROY:
		mChannel.Broadcast(Scheduler::StopEvent());
		return 0;
		break;

	case WM_CLOSE:
		mChannel.Broadcast(Scheduler::StopEvent());
		return 0;
		break;

	case WM_KEYDOWN:
		Keyboard::Callback((int)w, 0);
		return 0;
		break;

	case WM_KEYUP:
		Keyboard::Callback((int)w, 1);
		return 0;
		break;

	case WM_CHAR:
		Keyboard::Callback((int)w, 2);
		return 0;
		break;

	case WM_MOUSEMOVE:
		Mouse::Callback(0, GET_X_LPARAM(l), GET_Y_LPARAM(l), 2);
		return 0;
		break;

	case WM_LBUTTONDOWN:
		Mouse::Callback(0, 0, 0, 0);
		return 0;
		break;

	case WM_LBUTTONUP:
		Mouse::Callback(0, 0, 0, 1);
		return 0;
		break;

	case WM_RBUTTONDOWN:
		Mouse::Callback(1, 0, 0, 0);
		return 0;
		break;

	case WM_RBUTTONUP:
		Mouse::Callback(1, 0, 0, 1);
		return 0;
		break;

	case WM_SIZE:
		mChannel.Broadcast(Resize(LOWORD(l), HIWORD(l)));
		return 0;
		break;

	case WM_MOUSEWHEEL:
		Mouse::Callback((int)GET_WHEEL_DELTA_WPARAM(w), 0, 0, 3);
		return 0;
		break;

	default:
		return DefWindowProc(hwnd, msg, w, l);
	}

	return DefWindowProc(hwnd, msg, w, l);
}
#endif

void Window::Create()
{
#ifdef _WIN32
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "SkyeCuillin";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		mLog->Print("Engine::Window", "Cannot register window class");
	}

	RECT rect;
	SetRect(&rect, 0, 0, mOptions->Get<int>("Window.Size.Width"), mOptions->Get<int>("Window.Size.Height"));
	AdjustWindowRectEx(&rect, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, false, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);

	mHwnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, 
		"SkyeCuillin", 
		mOptions->Get<std::string>("Window.Title").c_str(),
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
		0,
		0,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL, 
		NULL, 
		mInstance,
		this);

	if (mHwnd == NULL)
	{
		mLog->Print("Engine::Window", "Cannot create window");
	}

	mDeviceContext = GetDC(mHwnd);

	ShowWindow(mHwnd, SW_SHOW);
	UpdateWindow(mHwnd);
#else
	// Connect to X-server
	const char* displayName = 0;
	mDisplay = XOpenDisplay(displayName);
	if (!mDisplay)
	{
		g_mLogger.SetMessageLevel(Logger::LOG_FATAL);
		g_mLogger.mPrefix = "[Window]";
		g_mLogger.mPostfix = "";
		g_mLogger << "Can not connect to X-server";
		g_mLogger.Flush();
		exit(1);
	}

	// Make sure, that X server has OpenGL GLX extensions
	int errorBase;
	int eventBase;
	bool success = glXQueryExtension(mDisplay, &errorBase, &eventBase);
	if (!success)
	{
		g_mLogger.SetMessageLevel(Logger::LOG_FATAL);
		g_mLogger.mPrefix = "[Window]";
		g_mLogger.mPostfix = "";
		g_mLogger << "GLX extensions not found";
		g_mLogger.Flush();
		exit(1);
	}

	// Query GLX version, must be grater than or equal to 1.3
	int glxMajor;
	int glxMinor;
	if (!glXQueryVersion(mDisplay, &glxMajor, &glxMinor))
	{
		g_mLogger.SetMessageLevel(Logger::LOG_FATAL);
		g_mLogger.mPrefix = "[Window]";
		g_mLogger.mPostfix = "";
		g_mLogger << "Can not query GLX version";
		g_mLogger.Flush();
		exit(1);
	}

	if (glxMajor < 1 || (glxMajor >= 1 && glxMinor < 3))
	{
		g_mLogger.SetMessageLevel(Logger::LOG_FATAL);
		g_mLogger.mPrefix = "[Window]";
		g_mLogger.mPostfix = "";
		g_mLogger << "GLX version must be at least 1.3, detected version: " << glxMajor << "." << glxMinor << ", please update your GLX";
		g_mLogger.Flush();
		exit(1);
	}

	g_mLogger.SetMessageLevel(Logger::LOG_INFO);
	g_mLogger.mPrefix = "[Window]";
	g_mLogger.mPostfix = "";
	g_mLogger << "GLX version is: " << glxMajor << "." << glxMinor;

	// Construction of GLX renderer
	static int attributes[] =
	{
		GLX_X_RENDERABLE, True,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		GLX_STENCIL_SIZE, 8,
		GLX_DOUBLEBUFFER, True,
		None
	};

	// Get All framebuffer configs and their count
	int fbcount;
	int fbmatch = -1;
	GLXFBConfig *fbconfigs = glXChooseFBConfig(mDisplay, 0, attributes, &fbcount);

	if (!fbconfigs)
	{
		g_mLogger.SetMessageLevel(Logger::LOG_FATAL);
		g_mLogger.mPrefix = "[Window]";
		g_mLogger.mPostfix = "";
		g_mLogger << "Can not get any frame buffer configurations, check your graphics drivers";
		g_mLogger.Flush();
		exit(1);
	}

	// Loop through all possible frame-buffer configs and find the one we want
	for (int i = 0; i < fbcount; i++)
	{
		XVisualInfo* visualInfo = glXGetVisualFromFBConfig(mDisplay, fbconfigs[i]);

		if (visualInfo)
		{
			int sb, samples;

			glXGetFBConfigAttrib(mDisplay, fbconfigs[i], GLX_SAMPLE_BUFFERS, &sb);
			glXGetFBConfigAttrib(mDisplay, fbconfigs[i], GLX_SAMPLES, &samples);

			fbmatch = i;

			if (config->GetInt("Window.Multisample") == samples)
			{
				fbmatch = i;
				XFree(visualInfo);
				break;
			}
		}

		XFree(visualInfo);
	}

	if (fbmatch == -1)
	{
		g_mLogger.SetMessageLevel(Logger::LOG_FATAL);
		g_mLogger.mPrefix = "[Window]";
		g_mLogger.mPostfix = "";
		g_mLogger << "Can not create frame buffer matching wanted configuration";
		g_mLogger.Flush();
		exit(1);
	}

	GLXFBConfig fbconfig = fbconfigs[fbmatch];

	XFree(fbconfigs);

	// Get visual information from framebuffer configuration
	XVisualInfo* visualInfo = glXGetVisualFromFBConfig(mDisplay, fbconfig);

	// Create colormap for the window
	mColorMap = XCreateColormap(mDisplay, RootWindow(mDisplay, visualInfo->screen), visualInfo->visual, AllocNone);

	XSetWindowAttributes winAttr;
	winAttr.colormap = mColorMap;
	winAttr.background_pixmap = None;
	winAttr.border_pixel = 0;
	winAttr.event_mask = StructureNotifyMask;

	// Create window
	mWidth = config->GetInt("Window.Width");
	mHeight = config->GetInt("Window.Height");

	mWindow = XCreateWindow(mDisplay,
		RootWindow(mDisplay, visualInfo->screen),
		0, 0,
		mWidth, mHeight,
		0,
		visualInfo->depth,
		InputOutput,
		visualInfo->visual,
		CWBorderPixel | CWColormap | CWEventMask,
		&winAttr);

	if (!mWindow)
	{
		g_mLogger.SetMessageLevel(Logger::LOG_ERROR);
		g_mLogger.mPrefix = "[Window]";
		g_mLogger.mPostfix = "";
		g_mLogger << "Failed to create window";
		g_mLogger.Flush();
		exit(1);
	}

	XFree(visualInfo);

	// Store window title and select window input
	XStoreName(mDisplay, mWindow, config->GetString("Window.Title").c_str());
	XSelectInput(mDisplay, mWindow, ButtonPressMask |
		ButtonReleaseMask |
		StructureNotifyMask |
		KeyPressMask |
		KeyReleaseMask |
		KeymapStateMask |
		MotionNotify |
		PointerMotionMask |
		ExposureMask |
		EnterNotify |
		LeaveNotify);

	XMapWindow(mDisplay, mWindow);

	Atom msgDelete = XInternAtom(mDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(mDisplay, mWindow, &msgDelete, 1);

	// Create OpenGL context
	const char* glxext = glXQueryExtensionsString(mDisplay, DefaultScreen(mDisplay));

	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

	if (!IsExtensionSupported(glxext, "GLX_ARB_create_context") || !glXCreateContextAttribsARB)
	{
		g_mLogger.SetMessageLevel(Logger::LOG_INFO);
		g_mLogger.mPrefix = "[Window]";
		g_mLogger.mPostfix = "";
		g_mLogger << "Can not create OpenGL context with attributes, creating legacy 2.0 context";

		mContext = glXCreateNewContext(mDisplay, fbconfig, GLX_RGBA_TYPE, 0, True);
	}
	else
	{
		int ctxattr[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, config->GetInt("Window.OpenGL.Major"),
			GLX_CONTEXT_MINOR_VERSION_ARB, config->GetInt("Window.OpenGL.Minor"),
			GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
			None
		};

		mContext = glXCreateContextAttribsARB(mDisplay, fbconfig, 0, True, ctxattr);

		// If unsuccessful, attemp to create legacy context
		if (!mContext)
		{
			g_mLogger.SetMessageLevel(Logger::LOG_INFO);
			g_mLogger.mPrefix = "[Window]";
			g_mLogger.mPostfix = "";
			g_mLogger << "Can not create OpenGL context version " << config->GetInt("Window.OpenGL.Major") << "." << config->GetInt("Window.OpenGL.Minor") << ", creating legacy 2.0 context";

			mContext = glXCreateNewContext(mDisplay, fbconfig, GLX_RGBA_TYPE, 0, True);
		}
	}

	XSync(mDisplay, False);

	glXMakeCurrent(mDisplay, mWindow, mContext);

	// Context creation failed
	if (!mContext)
	{
		g_mLogger.SetMessageLevel(Logger::LOG_FATAL);
		g_mLogger.mPrefix = "[Window]";
		g_mLogger.mPostfix = "";
		g_mLogger << "Can not create OpenGL context, check your graphics driver";
		g_mLogger.Flush();
		exit(1);
	}
	else
	{
		g_mLogger.SetMessageLevel(Logger::LOG_INFO);
		g_mLogger.mPrefix = "[Window]";
		g_mLogger.mPostfix = "";
		g_mLogger << "Successfully created OpenGL " << config->GetInt("Window.OpenGL.Major") << "." << config->GetInt("Window.OpenGL.Minor") << " context";
	}

	XFlush(mDisplay);

#endif
}

void Window::Destroy()
{
#ifdef _WIN32
#else
	glXMakeCurrent(mDisplay, 0, 0);
	glXDestroyContext(mDisplay, mContext);
	XDestroyWindow(mDisplay, mWindow);
	XFreeColormap(mDisplay, mColorMap);
	XCloseDisplay(mDisplay);
#endif
}

void Window::Paint()
{
#ifdef _WIN32
	SwapBuffers(mDeviceContext);
#else
	glXSwapBuffers(mDisplay, mWindow);
#endif
}

void Window::Process()
{
#ifdef _WIN32
	MSG msg;
	BOOL ret;

	while ((ret = PeekMessage(&msg, mHwnd, 0, 0, PM_REMOVE)) != 0)
	{
		if (ret == -1)
		{
			mLog->Print("Engine::Window", "Window::Process - received -1 inside message loop");
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
#else
	XEvent event;
	KeySym key;

	while (XPending(this->mDisplay) > 0)
	{
		XNextEvent(mDisplay, &event);

		switch (event.type)
		{
		case ClientMessage:
			if (strcmp(XGetAtomName(mDisplay, event.xclient.message_type), "WM_PROTOCOLS") == 0)
			{
				mChannel.Broadcast(Scheduler::StopEvent());
			}

			/* Key pressed */
		case KeyPress:
			key = XLookupKeysym(&event.xkey, 0);
			Keyboard::Callback((int)key, 0);
			break;

			/* Key released */
		case KeyRelease:
			if (event.type == KeyRelease && XEventsQueued(this->mDisplay, QueuedAfterReading))
			{
				XEvent nextEvent;
				XPeekEvent(this->mDisplay, &nextEvent);

				if (nextEvent.type == KeyPress && nextEvent.xkey.time == event.xkey.time && nextEvent.xkey.keycode == event.xkey.keycode)
				{
				}
				else
				{
					key = XLookupKeysym(&event.xkey, 0);
					Keyboard::Callback((int)key, 1);
				}
			}
			else
			{
				key = XLookupKeysym(&event.xkey, 0);
				Keyboard::Callback((int)key, 1);
			}
			break;

			/* Button pressed */
		case ButtonPress:
			switch (event.xbutton.button)
			{
			case 1:
				Mouse::Callback(0, 0, 0, 0);
				break;

			case 2:
				Mouse::Callback(1, 0, 0, 0);
				break;

			case 3:
				Mouse::Callback(2, 0, 0, 0);
				break;

			case 4:
				Mouse::Callback(3, 0, 0, 0);
				break;

			case 5:
				Mouse::Callback(4, 0, 0, 0);
				break;

			default:
				break;
			}
			break;

			/* Button released */
		case ButtonRelease:
			switch (event.xbutton.button)
			{
			case 1:
				Mouse::Callback(0, 0, 0, 1);
				break;

			case 2:
				Mouse::Callback(1, 0, 0, 1);
				break;

			case 3:
				Mouse::Callback(2, 0, 0, 1);
				break;

			default:
				break;
			}
			break;

			/* Mouse motion */
		case MotionNotify:
			Mouse::Callback(0, event.xmotion.x, event.xmotion.y, 2);
			break;

		default:
			break;
		}
	}
#endif
}

void Window::SetCursorPosition(int x, int y)
{
#ifdef _WIN32
	SetCursorPos(x, y);
#else
	XWarpPointer(mDisplay, None, mWindow, 0, 0, 0, 0, x, y);
#endif
}

void Window::GetCursorPosition(int* x, int* y)
{
	POINT pos;
	GetCursorPos(&pos);
	*x = pos.x;
	*y = pos.y;
}

void Window::GetSize(int* width, int* height)
{
#ifdef _WIN32
	RECT rect;
	if(GetClientRect(mHwnd, &rect))
	{
		*width = rect.right - rect.left;
		*height = rect.bottom - rect.top;
	}
	else
	{
		mLog->Print("Engine::Window", "Failed at Engine::Window::GetSize");
	}
#else
	XWindowAttributes attr;
	XGetWindowAttributes(mDisplay, mWindow, &attr);

	*width = attr.width;
	*height = attr.height;
#endif
}

void Window::GetPosition(int* x, int* y)
{
#ifdef _WIN32
	RECT rect;
	if(GetWindowRect(mHwnd, &rect))
	{
		*x = rect.left;
		*y = rect.top;
	}
	else
	{
		mLog->Print("Engine::Window", "Failed at Engine::Window::GetPosition");
	}
#else
	XWindowAttributes attr;
	XGetWindowAttributes(mDisplay, mWindow, &attr);

	*x = attr.x;
	*y = attr.y;
#endif
}
