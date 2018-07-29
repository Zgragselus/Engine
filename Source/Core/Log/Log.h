///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Log.h
//
// Following file contains class handling logging streams.
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __LOG_H__
#define __LOG_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/DllDef.h"
#include "Core/Time/Time.h"
#include "Core/Memory/Memory.h"
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Log class, allows for logging application debugging information and errors.
	/// </summary>
	class Log
	{
	private:
		/// <summary>LogHelper class for logging, makes sure that each line is outputted correctly</summary>
		class LogHelper
		{
		private:
			// Log to which LogHelper is bind
			Log* mParent;

		public:
			/// <summary>Constructor binding LogHelper to Log</summary>
			/// <param name="parent">Log to which we bind</param>
			LogHelper(Log* parent);

			/// <summary>Destructor, flushes the log message, ends line</summary>
			~LogHelper();

			/// <summary>Pass value into LogHelper, passes it further into Log buffer</summary>
			/// <param name="value">Value or string which we are sending into log</param>
			template<typename T>
			LogHelper& operator << (const T& value)
			{
				mParent->mBuffer << value;
				return *this;
			}

			/// <summary>Operator for handling endl symbol</summary>
			/// <param name="fn">Endl isn't a symbol, but a function pointer</param>
			LogHelper& operator << (std::ostream& (*fn)(std::ostream& os));
		};

		friend class LogHelper;

		std::stringstream mBuffer;	// Actual buffer for holding the string until it is flushed
		std::vector<std::pair<unsigned int, std::ostream*> > mOutputs;	// Multiple outputs
		unsigned int mErrorLevel;	// Current message log level (INFO, WARNING, ERROR ???)
		std::string mPrefix;			// Prefix string
		std::string mPostfix;			// Postfix string

		/// <summary>The method flushes the buffer to ostream</summary>
		void flush();

		/// <summary>Reset method</summary>
		void reset();

	public:
		/// <summary>Logging level</summary>
		enum ErrorLevel
		{
			LOG_ERROR = 1 << 0,		// Errors
			LOG_WARNING = 1 << 1,	// Warnings
			LOG_DEBUG = 1 << 2,		// Debug informations

			LOG_DEFAULT = LOG_ERROR | LOG_WARNING | LOG_DEBUG,
			LOG_ALL = ~0x0
		};

		/// <summary>Creates log file and sets logging up</summary>
		/// <param name="errorLevel">Logging level</param>
		Log(unsigned int errorLevel = LOG_DEFAULT);

		/// <summary>Finishes file, clears out used memory</summary>
		~Log();

		/// <summary>
		/// The following method puts prefix of log line into buffer, followed by value, instatiates
		/// the LogHelper class, which upon descruction creates new line
		/// The desctruction occurs when the code line ends with ;, after last use of operator << on a
		/// single line of code.
		/// </summary>
		/// <param name="value">Value to insert into log</param>
		/// <return>Loghelper type. correctly finishes printing</return>
		template<typename T>
		LogHelper operator << (const T& value)
		{
			mBuffer << mPrefix << " " << value;
			return LogHelper(this);
		}

		/// <summary>
		/// Specification for function pointer
		/// Because 'endl' is a function pointer, this specialization handles it
		/// </summary>
		/// <param name="fn">Function pointer to print</param>
		/// <return>Loghelper type. correctly finishes printing</return>
		LogHelper operator << (std::ostream& (*fn)(std::ostream& os));

		/// <summary>
		/// Add output for specific error level
		/// For any specific ErrorLevel you can use different outputs. F.e outputting debug information
		/// into debug.log file, errors into error.log and info simuateously.log, etc.
		/// </summary>
		/// <param name="output">File into which we log</param>
		/// <param name="errorLevel">Output level</param>
		void AddOutput(std::ostream* output, unsigned int errorLevel);

		/// <summary>Flush all the streams</summary>
		void Flush();

		/// <summary> Removes logging output.</summary>
		/// <param name="output">File into which we log</param>
		void RemoveOutput(std::ostream* output);

		/// <summary> Get total number of outputs.</summary>
		/// <return>The total number of outputs</return>
		unsigned int GetOutputsCount() const;

		/// <summary> Set current output level.</summary>
		void SetMessageLevel(unsigned int errorLevels);

		/// <summary> Get current output level.</summary>
		/// <return>Level of messages <see>ErrorLevel</see></return>
		unsigned int GetMessageLevel();

		/// <summary>Set postfix</summary>
		/// <param name="postfix">Postfix string</param>
		void SetPostfix(const std::string& postfix)
		{
			mPostfix = postfix;
		}

		/// <summary>Set prefix</summary>
		/// <param name="prefix">Prefix string</param>
		void SetPrefix(const std::string& prefix)
		{
			mPrefix = prefix;
		}

		/// <summary>Print</summary>
		/// <param name="c">Prefix string</param>
		/// <param name="a">Main string</param>
		void Print(const std::string& c, const std::string& a)
		{
			SetPrefix(c);
			SetPostfix("");
			(*this) << " " << Time::GetTime() << " " << a;
		}

		ALIGNED_NEW_DELETE("Engine::Core::Log")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
