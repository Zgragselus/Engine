///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Common.h
//
// Common math constants and functions
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __COMMON_MATH__H__
#define __COMMON_MATH__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/DllDef.h"
#include <math.h>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Math class. The following class contains constants and common math functions.
	/// </summary>
	class Math
	{
	private:
		/// <summary>32-bit integer log2 table</summary>
		static unsigned int Log2Table32[32];

		/// <summary>64-bit integer log2 table</summary>
		static unsigned int Log2Table64[64];

	public:
		/// <summary>PI number</summary>
		static float PI;
		/// <summary>Euler's number</summary>
		static float E;

		/// <summary>
		/// Convert degrees to radians
		/// </summary>
		/// <param name="degrees">Value to convert into radians</param>
		static float DegToRad(float degrees)
		{
			return (degrees * Math::PI / 180.0f);
		}

		/// <summary>
		/// Convert radians into degrees
		/// </summary>
		/// <param name="radians">Value to convert into degrees</param>
		static float RadToDeg(float radians)
		{
			return (radians * 180.0f / Math::PI);
		}

		/// <summary>
		/// Maximum of given two values (float)
		/// <summary>
		/// <param name="a">Value 1</param>
		/// <param name="b">Value 2</param>
		static inline float Max(float a, float b)
		{
			return fmaxf(a, b);
		}

		static inline int Max(int a, int b)
		{
			return a > b ? a : b;
		}

		static inline int Max(int a, unsigned int b)
		{
			return a > (int)b ? a : (int)b;
		}

		static inline int Max(unsigned int a, int b)
		{
			return (int)a > b ? (int)a : b;
		}

		static inline unsigned int Max(unsigned int a, unsigned int b)
		{
			return a > b ? a : b;
		}

		/// <summary>
		/// Minimum of given two values (float)
		/// <summary>
		/// <param name="a">Value 1</param>
		/// <param name="b">Value 2</param>
		static inline float Min(float a, float b)
		{
			return fminf(a, b);
		}

		static inline int Min(int a, int b)
		{
			return a < b ? a : b;
		}

		static inline int Min(int a, unsigned int b)
		{
			return a < (int)b ? a : (int)b;
		}

		static inline int Min(unsigned int a, int b)
		{
			return (int)a < b ? (int)a : b;
		}

		static inline unsigned int Min(unsigned int a, unsigned int b)
		{
			return a < b ? a : b;
		}
		
		static inline unsigned int log2(unsigned int value)
		{
			value |= value >> 1;
			value |= value >> 2;
			value |= value >> 4;
			value |= value >> 8;
			value |= value >> 16;
			return Log2Table32[(unsigned int)(value * 0x07C4ACDD) >> 27];
		}

		static inline unsigned long long log2(unsigned long long value)
		{
			value |= value >> 1;
			value |= value >> 2;
			value |= value >> 4;
			value |= value >> 8;
			value |= value >> 16;
			value |= value >> 32;
			return Log2Table64[((unsigned long long)((value - (value >> 1)) * 0x07EDD5E59A4E28C2)) >> 58];
		}

		/// <summary>
		/// Is passed string a floating point value?
		///
		/// By definition, floating point value can be either of:
		/// .0 .0f 0. 0.f 0.0f
		/// Where 0 stands for any number of numeric characters
		/// <summary>
		/// <param name="str">Trimmed string that can contain a floating point value</param>
		static inline bool IsFloat(const std::string& str)
		{
			// Find decimal dot, 'f'
			size_t ptpos = str.find('.');
			size_t fpos = str.find('f');

			size_t count = str.length();

			// 'f' is voluntary and must be last
			if (fpos != std::string::npos)
			{
				// Use one less character for loop, as we don't need to test 'f' anymore
				count--;

				if (fpos != str.length() - 1)
				{
					return false;
				}
			}

			// Everything except of '.' has to be a digit
			for (unsigned int i = 0; i < count; i++)
			{
				if (i == ptpos)
				{
					continue;
				}

				if (!isdigit(str[i]))
				{
					return false;
				}
			}

			return true;
		}

		/// <summary>
		/// Is passed string an integer value
		///
		/// Integer value is any string composed of digits only
		/// <summary>
		/// <param name="str">Trimmed string that can contain an integer value</param>
		static inline bool IsInteger(const std::string& str)
		{
			size_t count = str.length();

			// Everything has to be a digit
			for (unsigned int i = 0; i < count; i++)
			{
				if (!isdigit(str[i]))
				{
					return false;
				}
			}

			return true;
		}

		/// <summary>
		/// Convert string to float (assuming it contains it)
		/// </summary>
		/// <param name="str">String to convert</param>
		static inline float ToFloat(const std::string& str)
		{
			return (float)atof(str.c_str());
		}

		/// <summary>
		/// Convert string to integer (assuming it contains it)
		/// </summary>
		/// <param name="str">String to convert</param>
		static inline int ToInteger(const std::string& str)
		{
			return atoi(str.c_str());
		}

		/// <summary>
		/// Convert float to string
		/// </summary>
		/// <param name="v">Float value</param>
		static inline std::string ToString(float v)
		{
			return std::to_string(v);
		}

		/// <summary>
		/// Convert integer to string
		/// </summary>
		/// <param name="v">Integer value</param>
		static inline std::string ToString(int v)
		{
			return std::to_string(v);
		}

		/// <summary>
		/// Convert pointer to string
		/// </summary>
		/// <param name="v">Pointer value</param>
		static inline std::string ToString(size_t v)
		{
			char tmp[32];
			snprintf(tmp, sizeof(tmp), "%lx", (unsigned long)v);
			return std::string(tmp);
		}
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif