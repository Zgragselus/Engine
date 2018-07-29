///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Memory.h
//
// Following file contains base definitions for memory class
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MEMORY_H__
#define __MEMORY_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/DllDef.h"
#include <string>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Procedure definition

namespace Engine
{
	/// <summary>Prints out the memory information</summary>
	/// <param name="output">Stream into which we will print</param>
	void MemoryDump(std::ostream& output);

	/// <summary>Prints given number into string in HEX format with 0x in the beginning</summary>
	/// <param name="ptr">Memory pointer to print</param>
	inline std::string PrintPointer(void* ptr)
	{
		std::stringstream ss;
		ss << "0x" << std::hex << static_cast<void*>(ptr);
		return ss.str();
	}

	/// <summary>Aligns value to certain alignment (e.g. making it divisible by alignment)</summary>
	inline size_t AlignTo(size_t num, size_t alignment)
	{
		return ((num + alignment - 1) / alignment) * alignment;
	}

#ifdef _DEBUG
	/// <summary>Allocates memory with specified size in bytes aligned on specific boundaries</summary>
	/// <param name="size">Memory size in bytes</param>
	/// <param name="alignment">Memory alignment boundaries</param>
	/// <return>Pointer to allocated memory on success, NULL on failure</return>
	void* SharedMalloc(size_t size, size_t alignment, const char* classname);

	/// <summary>Frees memory aligned on specific boundaries</summary>
	/// <param name="ptr">Pointer to memory to be freed</param>
	void SharedFree(void* ptr, const char* classname);
#else
	/// <summary>Allocates memory with specified size in bytes aligned on specific boundaries</summary>
	/// <param name="size">Memory size in bytes</param>
	/// <param name="alignment">Memory alignment boundaries</param>
	/// <return>Pointer to allocated memory on success, NULL on failure</return>
	inline void* SharedMalloc(size_t size, size_t alignment, const char*)
	{
		if (alignment == 1)
		{
			return malloc(size);
		}

		if (!(alignment & (alignment - 1)) && alignment < sizeof(void*))
		{
			alignment = sizeof(void*);
		}

		void* allocated;

#ifdef _WIN32
		allocated = _aligned_malloc(size, alignment);
#elif defined __MINGW32__
		allocated = __mingw_aligned_malloc(bytes, alignment);
#else
		if (posix_memalign(&allocated, alignment, bytes) != 0)
		{
			return 0;
		}
#endif

		return allocated;
	}

	/// <summary>Frees memory aligned on specific boundaries</summary>
	/// <param name="ptr">Pointer to memory to be freed</param>
	inline void SharedFree(void* ptr, const char*)
	{
#ifdef _WIN32
		_aligned_free(ptr);
#elif defined __MINGW32__
		__mingw_aligned_free(ptr);
#else
		free(ptr);
#endif
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Globals

/// <summary>Global definition for objects aligned at 16 bytes boundary (SSE)</summary>
#define ALIGNED_NEW_DELETE(classname) \
	void* operator new(size_t size)\
{\
	return Engine::SharedMalloc(size, 16, classname); \
}\
	\
	void* operator new[](size_t size)\
{\
	return Engine::SharedMalloc(size, 16, classname); \
}\
	\
	void operator delete(void *ptr)\
{\
	Engine::SharedFree(ptr, classname); \
}\
	\
	void operator delete[](void *ptr)\
{\
	Engine::SharedFree(ptr, classname); \
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif