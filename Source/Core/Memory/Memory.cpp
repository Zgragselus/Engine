///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Memory.cpp
//
// Following file contains debugging version for Memory functions
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

#include "Memory.h"
#include "Core/Math/Numeric/Common.h"
#include <map>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Declaration section

using namespace Engine;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

#ifdef _DEBUG
/// <summary>Total number of memory allocations</summary>
size_t g_mMemoryAllocations = 0;

/// <summary>Total number of memory frees</summary>
size_t g_mMemoryFrees = 0;

/// <summary>Structure for tracking memory</summary>
struct MemoryRecord
{
	/// <summary>Size of allocated memory in bytes</summary>
	size_t mSize;

	/// <summary>Memory boundary alignment</summary>
	size_t mAlignment;

	/// <summary>ID of allocation</summary>
	size_t mMemoryAllocationId;

	/// <summary>Memory pointer</summary>
	void* mPointer;
};

/// <summary>Map holding memory string (generated) and memory record</summary>
std::map<std::string, MemoryRecord*> g_mMemoryMap;

/// <summary>Prints out the memory information</summary>
/// <param name="output">Stream into which we will print</param>
void Engine::MemoryDump(std::ostream& output)
{
	output << "Memory dump:" << std::endl;
	for (auto it = g_mMemoryMap.begin(); it != g_mMemoryMap.end(); it++)
	{
		// Get object type from unique ID string
		std::string objName = it->first.substr(it->first.find_first_of('_') + 1);
		objName = objName.substr(0, objName.find_last_of('_'));

		// Output the record
		output << "Record " << it->second->mMemoryAllocationId << " (" << objName << ")" << " Size: " <<
			it->second->mSize << " Aligned at: " << it->second->mAlignment << " At: " << it->second->mPointer << std::endl;
	}
}

/// <summary>Allocates memory with specified size in bytes aligned on specific boundaries</summary>
/// <param name="size">Memory size in bytes</param>
/// <param name="alignment">Memory alignment boundaries</param>
/// <return>Pointer to allocated memory on success, NULL on failure</return>
void* Engine::SharedMalloc(size_t size, size_t alignment, const char* classname)
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

	// Store memory record, set up all data
	MemoryRecord* m = (MemoryRecord*)malloc(sizeof(MemoryRecord));
	m->mSize = size;
	m->mAlignment = alignment;
	m->mMemoryAllocationId = g_mMemoryAllocations;
	m->mPointer = allocated;
	std::string name = std::string("SharedMalloc_") + std::string(classname) + std::string("_") + Math::ToString((size_t)allocated);
	g_mMemoryMap.insert(std::pair<std::string, MemoryRecord*>(name, m));
	g_mMemoryAllocations++;

	// Return pointer
	return allocated;
}

/// <summary>Frees memory aligned on specific boundaries</summary>
/// <param name="ptr">Pointer to memory to be freed</param>
void Engine::SharedFree(void* ptr, const char* classname)
{
	// Remove pointer from memory map
	std::string name = std::string("SharedMalloc_") + std::string(classname) + std::string("_") + Math::ToString((size_t)ptr);
	auto it = g_mMemoryMap.find(name);
	if (it != g_mMemoryMap.end())
	{
		free(it->second);
		g_mMemoryMap.erase(it);
	}

	// Free pointer
#ifdef _WIN32
	_aligned_free(ptr);
#elif defined __MINGW32__
	__mingw_aligned_free(ptr);
#else
	free(ptr);
#endif
}
#else
/// <summary>Prints out the memory information (Not in Release)</summary>
/// <param name="output">Stream into which we will print</param>
void Engine::MemoryDump(std::ostream& output)
{
}
#endif