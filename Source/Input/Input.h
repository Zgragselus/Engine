///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Input.h
//
// Following file contains definition of Input engine system
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __INPUT_H__
#define __INPUT_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Keyboard.h"
#include "Mouse.h"
#include "Core/System/System.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	class Input : public System
	{
	private:
		Keyboard* mKeyboard;		// Pointer to keyboard
		Mouse* mMouse;				// Pointer to mouse

	public:
		/// <summary>
		/// Constructor
		/// </summary>
		Input() :
			System("Input")
		{
			EnableUpdater();
		}

		/// <summary>
		/// Initialize system
		/// </summary>
		virtual bool Init()
		{
			mKeyboard = new Keyboard();
			mMouse = new Mouse();
			return true;
		}

		/// <summary>
		/// Update system
		/// </summary>
		virtual void Update()
		{
		}

		/// <summary>
		/// Shutdown system
		/// </summary>
		virtual void Shutdown()
		{
			delete mKeyboard;
			delete mMouse;
		}

		/// <summary>
		/// Gets mouse input
		/// </summary>
		Mouse* GetMouse()
		{
			return mMouse;
		}

		/// <summary>
		/// Gets keyboard input
		/// </summary>
		Keyboard* GetKeyboard()
		{
			return mKeyboard;
		}

		ALIGNED_NEW_DELETE("Engine::Input")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
