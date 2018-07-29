///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Plugin.h
//
// Class for run-time dynamic linking - loading and unloading libraries, and getting pointers to 
// their functions
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PLUGIN__H__
#define __PLUGIN__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Memory/Memory.h"
#include <Windows.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	///	
	/// </summary>
	class Plugin
	{
	private:
		HINSTANCE mDllHandle;

	public:
		/// <summary>
		/// Constructor, gets handle to the Dll (loads Dll if necessary)
		/// </summary>
		/// <param name="filename">Path to Dll file</param>
		Plugin(const std::string& filename);

		/// <summary>
		/// De-references the Dll (unloads if possible)
		/// </summary>
		~Plugin();

		/// <summary>
		/// Get address of procedure from library
		/// </summary>
		/// <param name="procedure">Name of the procedure we need address to</param>
		template <typename T>
		T GetProc(const std::string& procedure)
		{
			return reinterpret_cast<T>GetProcAddress(mDllHandle, procedure.c_str());
		}

		ALIGNED_NEW_DELETE("Engine::Plugin")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif