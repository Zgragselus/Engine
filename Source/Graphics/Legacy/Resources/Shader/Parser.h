///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Parser.h
//
// Following file contains class parsing custom shader files. Adding correct pre-processing, 
// include handling.
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SHADER_PARSER__H__
#define __SHADER_PARSER__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Log/Log.h"
#include <map>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Shader parser allows to use extended mode shaders, that allows for:
// 1) Include files (with masking off unused functions)
// 2) Removing unused #ifdef branches
// 3) Having all shader types inside one file
// 4) Having multiple versions within one file
//
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
	/// <summary>Variable for debug saving of shaders</summary>
	const char *ShaderTypeDebugName[];

	/// <summary>
	/// Shader parser class, contains all functions for parsing our custom shader files
	/// </summary>
	class ShaderParser
	{
	private:
		/// <summary>
		/// Directory of shader file, used for correctly include
		/// </summary>
		std::string mDirectory;

		/// <summary>Log output</summary>
		Log* mLog;

		/// <summary>Line type enumeration</summary>
		enum LineType
		{
			LINE_CODE,						/// Code line
			LINE_MACRO_DEFINE,				/// #define line
			LINE_MACRO_IFDEF,				/// #ifdef line
			LINE_MACRO_IFNDEF,				/// #ifndef line
			LINE_MACRO_ELSE,				/// #else line
			LINE_MACRO_ELIF,				/// #elif line
			LINE_MACRO_ENDIF,				/// #endif line
			LINE_INCLUDE,					/// #include line
			LINE_QUALITY,					/// #quality line
			LINE_SHADER_SPECIFICATION,		/// [SHADER TYPE] line
			LINE_SHADER_VERSION				/// #version line
		};

	public:
		/// <summary>Shader type enumeration</summary>
		enum ShaderType
		{
			SHADER_VERTEX = 0,		/// Vertex shader
			SHADER_TESS_CONTROL,	/// Tessellation control shader
			SHADER_TESS_EVAL,		/// Tessellation evaluation shader
			SHADER_GEOMETRY,		/// Geometry shader
			SHADER_FRAGMENT,		/// Fragment shader
			SHADER_COMPUTE			/// Compute shader
		};

		/// <summary>
		/// Parser constructor
		/// </summary>
		ShaderParser(Log* log);

		/// <summary>
		/// Following attribute contains parsed data for the shader, for different shader types
		/// it contains the version and shader code.
		/// </summary>
		std::map<ShaderType, std::map<int, std::vector<std::string> > > mParseData;

		/// <summary>
		/// Gets shader type based upon what is on line
		/// </summary>
		/// <param name="line">Text containing shader line definition</param>
		ShaderType GetShaderType(const std::string& line);

		/// <summary>
		/// Following method parses "#version XYZ" line, where XYZ represents version value
		/// </summary>
		/// <param name="line">Line containing version</param>
		int GetVersion(const std::string& line);

		/// <summary>
		/// Following method parses "#define EXPR" line, where EXPR is a defined keyword
		/// </summary>
		/// <param name="line">Line containing define</param>
		std::string GetDefine(const std::string& line);

		/// <summary>
		/// Following method takes line and returns it's type
		/// </summary>
		/// <param name="line">Input line</param>
		LineType GetPreprocessorLineType(const std::string& line);

		/// <summary>
		/// Reads file, outputting each line into the lines argument
		/// </summary>
		/// <param name="filename">File to read</param>
		/// <param name="lines">Resulting buffer of lines</param>
		bool ReadFile(const std::string& filename, std::vector<std::string>& lines);

		/// <summary>
		/// Remove comments from lines
		/// </summary>
		/// <param name="lines">All lines from shader file</param>
		void RemoveComments(std::vector<std::string>& lines);

		/// <summary>
		/// Following method processes all ifdef/else/elif/endif and define, removes parts of code based on this
		/// </summary>
		/// <param name="defines">Array of defines passed into the compiler as arguments</param>
		void ProcessIfdefs(const std::vector<std::string>& defines);

		/// <summary>
		/// Opens the shader file, parses it and returns true on success
		/// </summary>
		/// <param name="lines">All lines from shader file</param>
		bool ParseFile(const std::vector<std::string>& lines);

		/// <summary>
		/// Gets include file name from line
		/// </summary>
		/// <param name="line">The line containing #include FILE</param>
		std::string GetInclude(const std::string& line);

		/// <summary>
		/// Preprocesses include files, by copying there content into the file
		/// </summary>
		void PreproessIncludes(const std::vector<std::string>& includeDirs);

		/// <summary>
		/// Helper function to save shaders
		/// </summary>
		void DebugSave();

		/// <summary>
		/// Preprocesses whole shader file
		/// </summary>
		void Parse(const std::string& filename, const std::vector<std::string> defines, const std::vector<std::string> dirs);
	};
}

#endif