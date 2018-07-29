#include "Renderer.h"

using namespace Engine;

/// <summary>
/// Renderer constructor
/// </summary>
/// <param name="window">Pointer to window into which the renderer will be bound</param>
/// <param name="log">Pointer to log (error and debug output)</param>
/// <param name="constants">Pointer to options (constants instance)</param>
Renderer::Renderer(Engine::Window* window, Log* log, Constants* constants)
{
	mConstants = constants;
	mLog = log;
	mWindow = window;
	HDC hdc = mWindow->GetDeviceContext();

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),									// Size of pixel format descriptor
		1,																// Version
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,     // Must Support Double Buffering, OpenGL and drawing to window
		PFD_TYPE_RGBA,													// Request An RGBA Format
		24,																// Select Our Color Depth
		0, 0, 0, 0, 0, 0,												// Color Bits Ignored
		8,																// 8-bit Alpha Buffer
		0,																// Shift Bit Ignored
		0,																// No Accumulation Buffer
		0, 0, 0, 0,														// Accumulation Bits Ignored
		24,																// 24-bit Z-Buffer (Depth Buffer)
		8,																// 8-bit Stencil Buffer
		0,																// No Auxiliary Buffer
		PFD_MAIN_PLANE,													// Main Drawing Layer
		0,																// Reserved
		0, 0, 0															// Layer Masks Ignored
	};

	int pixelFormat = ChoosePixelFormat(hdc, &pfd);

	if (pixelFormat == 0)
	{
		mLog->Print("Engine::Graphics::Renderer", "Failed to initalize");
		return;
	}

	BOOL result = SetPixelFormat(hdc, pixelFormat, &pfd);

	if (!result)
	{
		mLog->Print("Engine::Graphics::Renderer", "Failed to initalize");
		return;
	}

	HGLRC tempContext = wglCreateContext(hdc);
	wglMakeCurrent(hdc, tempContext);

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, mConstants->Get<int>("Graphics.OpenGL.Version.Major"),
		WGL_CONTEXT_MINOR_VERSION_ARB, mConstants->Get<int>("Graphics.OpenGL.Version.Minor"),
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
	if (wglGetExtensionsStringARB != NULL)
	{
		const char* ext = wglGetExtensionsStringARB(hdc);

		if (strstr(ext, "WGL_ARB_create_context"))
		{
			PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
			if (wglCreateContextAttribsARB != NULL)
			{
				mContext = wglCreateContextAttribsARB(hdc, 0, attribs);
				wglMakeCurrent(NULL, NULL);
				wglDeleteContext(tempContext);
				wglMakeCurrent(hdc, mContext);
			}
			else
			{
				mContext = tempContext;
			}
		}
		else
		{
			mContext = tempContext;
		}
	}

	if (!mContext)
	{
		mLog->Print("Engine::Graphics::Renderer", "Failed to initalize");
		return;
	}

	int minor, major;
	CheckExtensions(&major, &minor);
}

/// <summary>
/// Renderer destructor
/// </summary>
Renderer::~Renderer()
{
	mWindow = NULL;

	wglMakeCurrent(NULL, NULL);
	if (mContext)
	{
		wglDeleteContext(mContext);
	}
}