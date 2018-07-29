///////////////////////////////////////////////////////////////////////////////////////////////////
//
// String.h
//
// Utility library for string functions
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __STRING__H__
#define __STRING__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include <string>
#include <sstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	///	A class for getting IDs in sequence, in case some are freed, they are stored in a queue so
	/// they can be re-used later
	/// </summary>
	class String
	{
	public:
		/// <summary>Serialization separator</summary>
		static char mSeparator;

		/// <summary>Left trim string</summary>
		/// <param name="s">String to be trimmed</param>
		static inline std::string& TrimLeft(std::string& s)
		{
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			return s;
		}

		/// <summary>Right trim string</summary>
		/// <param name="s">String to be trimmed</param>
		static inline std::string& TrimRight(std::string& s)
		{
			s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
			return s;
		}

		/// <summary>Trim string from left and right</summary>
		/// <param name="s">String to be trimmed</param>
		static inline std::string& Trim(std::string& s)
		{
			return TrimLeft(TrimRight(s));
		}

		/// <summary>Split string on delimiter</summary>
		/// <param name="s">String to be splitted</param>
		/// <param name="delim">Delimiter to split on</param>
		/// <param name="elems">Output list of elements</param>
		static std::vector<std::string>& Split(const std::string& s, char delim, std::vector<std::string>& elems)
		{
			std::stringstream ss(s);
			std::string item;

			while (std::getline(ss, item, delim))
			{
				elems.push_back(item);
			}

			return elems;
		}

		/// <summary>Split string on delimiter</summary>
		/// <param name="s">String to be splitted</param>
		/// <param name="delim">Delimiter to split on</param>
		static std::vector<std::string> Split(const std::string& s, char delim)
		{
			std::vector<std::string> elems;
			Split(s, delim, elems);
			return elems;
		}

		/// <summary>Joins vector of strings with newlines</summary>
		/// <param name="strings">Vector of strings</param>
		/// <param name="delim">Delimiter to join on</param>
		static std::string Join(const std::vector<std::string>& strings, char delim)
		{
			std::stringstream ss;

			for (const std::string& s : strings)
			{
				ss << s;
				ss << delim;
			}

			return ss.str();
		}

		/// <summary>Get name from path</summary>
		/// <param name="path">Path</param>
		static std::string Filename(const std::string& path)
		{
			// Find last '/' or '\\'
			size_t slashPos[2];
			slashPos[0] = path.find_last_of('/');
			slashPos[1] = path.find_last_of('\\');
			
			size_t begin = 0;
			if (slashPos[0] != std::string::npos)
			{
				begin = slashPos[0];
			}

			if (slashPos[1] != std::string::npos)
			{
				if (slashPos[1] > begin)
				{
					begin = slashPos[1];
				}
			}

			// Find '.'
			size_t dotPos = path.find_last_of('.');

			return path.substr(begin + 1, dotPos - begin - 1);
		}
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif