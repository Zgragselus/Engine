///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Functions.h
//
// Following file contains class for adding/retrieving and removing functions accessed by their
// name.
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/DllDef.h"
#include "Core/Memory/Memory.h"
#include <map>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// The following class contains functions map and methods to set them up and retrieve
	/// </summary>
	class Functions
	{
	protected:
		/// <summary>Map of functions (maps function name to function pointer)</summary>
		std::map<std::string, void*>* mFunctions;

	public:
		/// <summary>Creates Functions object</summary>
		Functions()
		{
			mFunctions = new std::map<std::string, void*>();
		}

		/// <summary>Destroys Functions object</summary>
		~Functions()
		{
			delete mFunctions;
		}

		/// <summary>Adds new function with specified name pointing to specified target function</summary>
		/// <param name="functionName">Function name</param>
		/// <param name="functionPtr">Function pointer</param>
		void Add(const std::string& functionName, void* functionPtr)
		{
			mFunctions->insert(std::pair<std::string, void*>(functionName, functionPtr));
		}

		/// <summary>Retrieves pointer (casted to templated type) to function with given name</summary>
		/// <param name="functionName">Function name</param>
		template <typename T>
		T Get(const std::string& functionName)
		{
			auto it = mFunctions->find(functionName);
			if (it != mFunctions->end())
			{
				return reinterpret_cast<T>(it->second);
			}
			else
			{
				return NULL;
			}
		}

		ALIGNED_NEW_DELETE("Engine::Core::Functions")
	};
}

/// <summary>Global functions space</summary>
extern Engine::Functions* g_mFunctions;

#endif