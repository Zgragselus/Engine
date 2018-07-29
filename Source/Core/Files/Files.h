 ///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Files.h
//
// Following file contains helper class for getting information about files
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FILES__H__
#define __FILES__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/DllDef.h"
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Helper class for working with files (their created time, last access time and last modification time)
	/// </summary>
	class Files
	{
	public:
		enum class Type
		{
			DIRECTORY = 0,
			FILE
		};

		struct Record
		{
			//Record* mParent;
			Type mType;
			std::string mName;
		};

		static void DirectoryRead(const std::string& directory, std::vector<std::string>& list);

		/// <summary>
		/// Get created time of file
		/// </summary>
		/// <param name="filename">Path and filename of the file</param>
		static unsigned long long GetCreatedTime(const std::string& filename);

		/// <summary>
		/// Get last access time of file
		/// </summary>
		/// <param name="filename">Path and filename of the file</param>
		static unsigned long long GetAccessTime(const std::string& filename);

		/// <summary>
		/// Get last modification time of file
		/// </summary>
		/// <param name="filename">Path and filename of the file</param>
		static unsigned long long GetWriteTime(const std::string& filename);

		/// <summary>
		/// Get whether file exists
		/// </summary>
		/// <param name="filename">Path and filename of the file</param>
		static bool Exists(const std::string& filename);

		/// <summary>
		/// Get whether passed in location is file or directory
		/// </summary>
		/// <param name="filename">Path and name of file or directory</param>
		static Type GetType(const std::string& recordname);

		/// <summary>
		/// Get current working directory
		/// </summary>
		static std::string GetCurrentWorkingDir();

		/// <summary>
		/// List all files in given directory
		/// </summary>
		/// <param name="directory">Directory from which the files are to be listed</param>
		/// <param name="result">Directory from which the files are to be listed</param>
		static int GetFilesInDir(const std::string& directory, std::vector<std::string>& result);

		/// <summary>
		/// Read whole file into byte array
		/// </summary>
		/// <param name="filename">Which file to read</param>
		/// <param name="size">Output variable, number of bytes read</param>
		static char* Read(const std::string& filename, unsigned long long* size);

		/// <summary>
		/// Write byte array into file
		/// </summary>
		/// <param name="filename">Which file to read</param>
		/// <param name="data">Pointer to data that are to be written</param>
		/// <param name="size">Size of the written data (in bytes)</param>
		static void Write(const std::string& filename, void* data, unsigned long long size);

		/// <summary>
		/// Get folder in which file is
		/// </summary>
		/// <param name="filename">Path to file which folder we want</param>
		static std::string GetFolder(const std::string& filename);
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif