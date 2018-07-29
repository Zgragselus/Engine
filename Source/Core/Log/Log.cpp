///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Log.cpp
//
// Following file implements methods defined in Log.h.
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

#include "Log.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Declaration section

using namespace Engine;

extern "C" Engine::Log* g_mLog;		// Single global instance of Log class

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

/// <summary>Constructor binding LogHelper to Log</summary>
/// <param name="parent">Log to which we bind</param>
Log::LogHelper::LogHelper(Log* parent) : mParent(parent) 
{
}

/// <summary>Destructor, flushes the log message, ends line</summary>
Log::LogHelper::~LogHelper()
{
	mParent->mBuffer << mParent->mPostfix << std::endl;
	mParent->flush();
}

/// <summary>Operator for handling endl symbol</summary>
/// <param name="fn">Endl isn't a symbol, but a function pointer</param>
Log::LogHelper& Log::LogHelper::operator << (std::ostream& (*fn)(std::ostream& os))
{
	std::ostringstream oss;
	fn(oss);
	return (*this) << oss.str();
}

/* Constructor */
Log::Log(unsigned int errorLevel) :
	mErrorLevel(errorLevel)
{
}

/* Destructor */
Log::~Log()
{
}

/* Operator <<, specification for function pointer */
// Because 'endl' is a function pointer, this specialization handles it
Log::LogHelper Log::operator << (std::ostream& (*fn)(std::ostream& os))
{
	std::ostringstream oss;
	fn(oss);
	return (LogHelper(this) << oss.str());
}

/// <summary>The method flushes the buffer to ostream</summary>
void Log::flush()
{
	for (std::vector<std::pair<unsigned int, std::ostream*> >::iterator it = mOutputs.begin(); it != mOutputs.end(); it++)
	{
		if(it->first & mErrorLevel)
		{
			(*it->second) << mBuffer.str();
		}
	}
	
	mBuffer.str("");
}

/// <summary>Reset method</summary>
void Log::reset()
{
	mErrorLevel = LOG_DEFAULT;
	mPrefix = "";
	mPostfix = "";
}

/// <summary>
/// Add output for specific error level
/// For any specific ErrorLevel you can use different outputs. F.e outputting debug information
/// into debug.log file, errors into error.log and info simuateously.log, etc.
/// </summary>
/// <param name="output">File into which we log</param>
/// <param name="errorLevel">Output level</param>
void Log::AddOutput(std::ostream* output, unsigned int errorLevel)
{
	mOutputs.push_back(std::pair<unsigned int, std::ostream*>(errorLevel, output));
}

/// <summary>Flush all the streams</summary>
void Log::Flush()
{
	for (std::vector<std::pair<unsigned int, std::ostream*> >::iterator it = mOutputs.begin(); it != mOutputs.end(); it++)
	{
		(*it->second).flush();
	}
}

/// <summary> Removes logging output.</summary>
/// <param name="output">File into which we log</param>
void Log::RemoveOutput(std::ostream* output)
{
	for (std::vector<std::pair<unsigned int, std::ostream*> >::iterator it = mOutputs.begin(); it != mOutputs.end(); it++)
	{
		if(it->second == output)
		{
			mOutputs.erase(it);
			break;
		}
	}
}

/// <summary> Get total number of outputs.</summary>
/// <return>The total number of outputs</return>
unsigned int Log::GetOutputsCount() const
{
	return (unsigned int)mOutputs.size();
}

/// <summary> Set current output level.</summary>
void Log::SetMessageLevel(unsigned int errorLevels)
{
	mErrorLevel = errorLevels;
}

/// <summary> Get current output level.</summary>
/// <return>Level of messages <see>ErrorLevel</see></return>
unsigned int Log::GetMessageLevel()
{
	return mErrorLevel;
}
