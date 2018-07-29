///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Renderer.h
//
// Renderer class, interface enabling OpenGL-based rendering into windows
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OPENGL__RENDERER__H__
#define __OPENGL__RENDERER__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#pragma comment(lib, "opengl32.lib")

#include "Window/Window.h"
#include "Core/Log/Log.h"
#include "Core/Constants/Constants.h"
#include "Extensions.h"
#include <Windows.h>
#include <gl/GL.h>
#include <gl/glext.h>
#include <gl/wglext.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Renderer class, holding actual window rendering context
	/// </summary>
	class Renderer
	{
	private:
		Log* mLog;				// Pointer to log instance
		Window* mWindow;		// Pointer to window instance
		Constants* mConstants;	// Pointer to options (constants instance)
		HGLRC mContext;			// Windows-system GL rendering context handle

	public:
		/// <summary>
		/// Renderer constructor
		/// </summary>
		/// <param name="window">Pointer to window into which the renderer will be bound</param>
		/// <param name="log">Pointer to log (error and debug output)</param>
		/// <param name="constants">Pointer to options (constants instance)</param>
		Renderer(Window* window, Log* log, Constants* constants);

		/// <summary>
		/// Renderer destructor
		/// </summary>
		~Renderer();

		/// <summary>
		/// Swap buffers (show render on display)
		/// </summary>
		void Swap()
		{
			if (mWindow->mSwappable)
			{
				HDC hdc = mWindow->GetDeviceContext();
				SwapBuffers(hdc);
			}
		}

		void SetViewport()
		{
			int w;
			int h;

			mWindow->GetSize(&w, &h);

			glViewport(0, 0, w, h);
		}

		/// <summary>
		/// Clear GL backbuffer (color, depth and stencil)
		/// </summary>
		/// <param name="color">RGBA color</param>
		/// <param name="depth">Default depth value</param>
		/// <param name="stencil">Default stencil value</param>
		void Clear(const float4& color, float depth, int stencil)
		{
			glClearStencil(stencil);
			glClearDepth(depth);
			glClearColor(color.x, color.y, color.z, color.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::Context")
	};
}

#endif