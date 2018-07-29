#include "Core.h"

using namespace Engine;

Core* Core::mInstance = 0;

/// <summary>
/// Get Instance of Core
/// </summary>
Core* Core::Instance()
{
	if (!mInstance)
	{
		mInstance = new Core();
	}

	return mInstance;
}

/// <summary>
/// Dispose of the instance
/// </summary>
void Core::Dispose()
{
	if (mInstance)
	{
		delete mInstance;
	}
}