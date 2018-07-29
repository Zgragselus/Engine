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

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include <Windows.h>
#include "Files.h"
#include <direct.h>
#include <fstream>
#include <cstdio>
#include "../Memory/Memory.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

using namespace Engine;

void Files::DirectoryRead(const std::string& directory, std::vector<std::string>& list)
{
	HANDLE find;
	WIN32_FIND_DATA data;
	
	find = FindFirstFile((directory + "/*").c_str(), &data);
	if (find != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (data.cFileName[0] == '.')
				{
					continue;
				}

				list.push_back(std::string(data.cFileName) + "/");
			}
			else if (data.dwFileAttributes)
			{
				list.push_back(std::string(data.cFileName));
			}
		} 
		while (FindNextFile(find, &data));

		FindClose(find);
	}
}

/// <summary>
/// Get created time of file
/// </summary>
/// <param name="filename">Path and filename of the file</param>
unsigned long long Files::GetCreatedTime(const std::string& filename)
{
	unsigned long long result = 0;
	HANDLE file;
	file = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (file != INVALID_HANDLE_VALUE)
	{
		FILETIME create, access, write;
		if (GetFileTime(file, &create, &access, &write))
		{
			result = (((unsigned long long)create.dwHighDateTime) << 32) + (unsigned long long)create.dwLowDateTime;
		}
	}
	CloseHandle(file);
	return result;
}

/// <summary>
/// Get last access time of file
/// </summary>
/// <param name="filename">Path and filename of the file</param>
unsigned long long Files::GetAccessTime(const std::string& filename)
{
	unsigned long long result = 0;
	HANDLE file;
	file = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (file != INVALID_HANDLE_VALUE)
	{
		FILETIME create, access, write;
		if (GetFileTime(file, &create, &access, &write))
		{
			result = (((unsigned long long)access.dwHighDateTime) << 32) + (unsigned long long)access.dwLowDateTime;
		}
	}
	CloseHandle(file);
	return result;
}

/// <summary>
/// Get last modification time of file
/// </summary>
/// <param name="filename">Path and filename of the file</param>
unsigned long long Files::GetWriteTime(const std::string& filename)
{
	unsigned long long result = 0;
	HANDLE file;
	file = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (file != INVALID_HANDLE_VALUE)
	{
		FILETIME create, access, write;
		if (GetFileTime(file, &create, &access, &write))
		{
			result = (((unsigned long long)write.dwHighDateTime) << 32) + (unsigned long long)write.dwLowDateTime;
		}
	}
	CloseHandle(file);
	return result;
}

/// <summary>
/// Get whether file exists
/// </summary>
/// <param name="filename">Path and filename of the file</param>
bool Files::Exists(const std::string& filename)
{
	std::ifstream ifs;
	ifs.open(filename.c_str());
	if (ifs.good())
	{
		ifs.close();
		return true;
	}
	else
	{
		ifs.close();
		return false;
	}
}

Files::Type Files::GetType(const std::string& recordname)
{
	DWORD attrib = GetFileAttributes(recordname.c_str());

	if (attrib & FILE_ATTRIBUTE_DIRECTORY)
	{
		return Files::Type::DIRECTORY;
	}
	else
	{
		return Files::Type::FILE;
	}
}

/// <summary>
/// Get current working directory
/// </summary>
std::string Files::GetCurrentWorkingDir()
{
	TCHAR tmp[MAX_PATH];
	GetModuleFileName(NULL, tmp, MAX_PATH);
	std::string file = std::string(tmp);
	file = file.substr(0, file.find_last_of("/\\"));
	return file;
}

/// <summary>
/// List all files in given directory
/// </summary>
/// <param name="directory">Directory from which the files are to be listed</param>
int Files::GetFilesInDir(const std::string& directory, std::vector<std::string>& result)
{
	HANDLE find;
	WIN32_FIND_DATA data;

	int counter = 0;

	find = FindFirstFile(directory.c_str(), &data);
	if (find != INVALID_HANDLE_VALUE)
	{
		do
		{
			counter++;
			result.push_back(directory + std::string(data.cFileName));
		} while (FindNextFile(find, &data));
		FindClose(find);
	}

	return counter;
}

/// <summary>
/// Read whole file into byte array
/// </summary>
/// <param name="filename">Which file to read</param>
/// <param name="size">Output variable, number of bytes read</param>
char* Files::Read(const std::string& filename, unsigned long long* size)
{
	char* contents = NULL;
	unsigned long long bytes = 0;

	// Open the file and get number of bytes
	FILE* file = fopen(filename.c_str(), "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		bytes = ftell(file);
		rewind(file);

		// Allocate memory and read whole file into it
		contents = (char*)SharedMalloc(sizeof(char) * (size_t)bytes + sizeof(char), 16, "FileContent");
		if (contents)
		{
			fread(contents, sizeof(char), (size_t)bytes, file);
			contents[bytes] = '\0';
		}
		fclose(file);
	}

	*size = bytes;
	return contents;
}

/// <summary>
/// Write byte array into file
/// </summary>
/// <param name="filename">Which file to read</param>
/// <param name="data">Pointer to data that are to be written</param>
/// <param name="size">Size of the written data (in bytes)</param>
void Files::Write(const std::string& filename, void* data, unsigned long long size)
{
	FILE* file = fopen(filename.c_str(), "wb");
	if (file)
	{
		fwrite(data, sizeof(char), (size_t)size, file);
		fclose(file);
	}
}

/// <summary>
/// Get folder in which file is
/// </summary>
/// <param name="filename">Path to file which folder we want</param>
std::string Files::GetFolder(const std::string& filename)
{
	size_t pos = filename.find_last_of('/');
	if (pos == std::string::npos)
	{
		pos = filename.find_last_of('\\');
	}

	std::string path = filename.substr(0, pos + 1);

	return path;
}