///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Parser.cpp
//
// Following file contains implementation of ShaderParser class methods.
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

#include "Parser.h"
#include "Core/Util/String.h"
#include "Core/Math/Numeric/Common.h"
#include <fstream>
#include <Windows.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Declaration section

using namespace Engine;

/// <summary>Variable for debug saving of shaders</summary>
const char *Engine::ShaderTypeDebugName[] =
{
	"Vertex",
	"TessellationControl",
	"TessellationEvaluation",
	"Geometry",
	"Fragment",
	"Compute"
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

/// <summary>
/// Parser constructor
/// </summary>
ShaderParser::ShaderParser(Log* log)
{
	mLog = log;
}

/// <summary>
/// Gets shader type based upon what is on line
/// </summary>
/// <param name="line">Text containing shader line definition</param>
ShaderParser::ShaderType ShaderParser::GetShaderType(const std::string& line)
{
	if (line.find("vertex shader") != std::string::npos)
	{
		return SHADER_VERTEX;
	}
	else if (line.find("tessellation control shader") != std::string::npos)
	{
		return SHADER_TESS_CONTROL;
	}
	else if (line.find("tessellation evaluation shader") != std::string::npos)
	{
		return SHADER_TESS_EVAL;
	}
	else if (line.find("geometry shader") != std::string::npos)
	{
		return SHADER_GEOMETRY;
	}
	else if (line.find("fragment shader") != std::string::npos)
	{
		return SHADER_FRAGMENT;
	}
	else if (line.find("compute shader") != std::string::npos)
	{
		return SHADER_COMPUTE;
	}

	mLog->Print("ShaderParser", std::string("Fatal error - invalid shader type"));
	return SHADER_COMPUTE;
}

/// <summary>
/// Following method parses "#version XYZ" line, where XYZ represents version value
/// </summary>
/// <param name="line">Line containing version</param>
int ShaderParser::GetVersion(const std::string& line)
{
	std::vector<std::string> tokens = String::Split(line, ' ');
	auto it = tokens.begin();
	it++;
	return Math::ToInteger((*it));
}

/// <summary>
/// Following method parses "#define EXPR" line, where EXPR is a defined keyword
/// </summary>
/// <param name="line">Line containing define</param>
std::string ShaderParser::GetDefine(const std::string& line)
{
	std::vector<std::string> tokens = String::Split(line, ' ');
	auto it = tokens.begin();
	it++;
	return *it;
}

/// <summary>
/// Following method takes line and returns it's type
/// </summary>
/// <param name="line">Input line</param>
ShaderParser::LineType ShaderParser::GetPreprocessorLineType(const std::string& line)
{
	// Trim the line
	std::string lineTrimmed = line;
	String::Trim(lineTrimmed);

	// Check macro tokens (define, ifdef, ifndef, else, elif, endif
	if (lineTrimmed.find("#define") == 0)
	{
		return LINE_MACRO_DEFINE;
	}
	else if (lineTrimmed.find("#ifdef") == 0)
	{
		return LINE_MACRO_IFDEF;
	}
	else if (lineTrimmed.find("#ifndef") == 0)
	{
		return LINE_MACRO_IFNDEF;
	}
	else if (lineTrimmed.find("#else") == 0)
	{
		return LINE_MACRO_ELSE;
	}
	else if (lineTrimmed.find("#elif") == 0)
	{
		return LINE_MACRO_ELIF;
	}
	else if (lineTrimmed.find("#endif") == 0)
	{
		return LINE_MACRO_ENDIF;
	}
	// Check include token
	else if (lineTrimmed.find("#include") == 0)
	{
		return LINE_INCLUDE;
	}
	// Check quality token
	else if (lineTrimmed.find("#quality") == 0)
	{
		return LINE_QUALITY;
	}
	// Check version token
	else if (lineTrimmed.find("#version") == 0)
	{
		return LINE_SHADER_VERSION;
	}

	// Specification line must have at least 2 symbols
	if (lineTrimmed.length() > 2)
	{
		// Specification
		if (lineTrimmed[0] == '[' && lineTrimmed[lineTrimmed.length() - 1] == ']')
		{
			return LINE_SHADER_SPECIFICATION;
		}

	}

	// Otherwise it's a code line
	return LINE_CODE;
}

/// <summary>
/// Reads file, outputting each line into the lines argument
/// </summary>
/// <param name="filename">File to read</param>
/// <param name="lines">Resulting buffer of lines</param>
bool ShaderParser::ReadFile(const std::string& filename, std::vector<std::string>& lines)
{
	// Open file
	std::ifstream f;
	f.open(filename, std::ifstream::in);

	// Log error on fail, and return
	if (!f.is_open())
	{
		mLog->Print("ShaderParser", std::string("Can't open file ") + filename);
		return false;
	}

	// Read all lines from file
	std::string line;
	while (std::getline(f, line))
	{
		// Trim line
		std::string lineTrimmed = line;
		String::Trim(lineTrimmed);

		// Store only non-empty lines and trimmed ones
		if (lineTrimmed.length() > 0)
		{
			lines.push_back(lineTrimmed);
		}
	}

	// Close file
	f.close();

	return true;
}

/// <summary>
/// Remove comments from lines
/// </summary>
/// <param name="lines">All lines from shader file</param>
void ShaderParser::RemoveComments(std::vector<std::string>& lines)
{
	// multi-line comment state
	bool multiLineCommentState = false;

	// Loop through all lines
	auto it = lines.begin();
	while (it != lines.end())
	{
		// If we're not in multi-line comment
		if (multiLineCommentState == false)
		{
			// There is no sense in controlling lines with less than 2 symbols ("/*" or "*/"), continue on next line then
			if ((*it).length() < 2)
			{
				it++;
				continue;
			}

			// Helper variable for "/*" and "*/" on single line
			size_t multiLineCommentBegin = 0;

			// Loop character by character
			for (size_t i = 1; i < (*it).length(); i++)
			{
				if ((*it)[i - 1] == '/')
				{
					// If we find "/*", multi-line comment begins
					if ((*it)[i] == '*')
					{
						multiLineCommentState = true;
						multiLineCommentBegin = i - 1;
					}
					// If we find "//" and we're not in multi-line comment, erase the rest of the line
					else if ((*it)[i] == '/' && multiLineCommentState == false)
					{
						(*it).erase(i - 1, std::string::npos);
						break;
					}
				}
				else if ((*it)[i - 1] == '*' && multiLineCommentState == true)
				{
					// If we find "*/" and we're in multi-line comment, erase what is between "/*" and "*/"
					if ((*it)[i] == '/')
					{
						multiLineCommentState = false;
						(*it).erase(multiLineCommentBegin, i - multiLineCommentBegin);

						// We erased what was in string, we have to restart search
						i = 0;
						continue;
					}
				}
			}

			// Trim the line, if it is empty, erase it
			String::Trim((*it));
			if ((*it).length() == 0)
			{
				it = lines.erase(it);
			}
			else
			{
				it++;
			}
		}
		// When we're in multi line comment
		else
		{
			size_t pos = (*it).find("*/");

			// If line is inside multi-line comment, remove line and continue
			if (pos == std::string::npos)
			{
				it = lines.erase(it);
			}
			// Otherwise remove everything from beginning of line to "*/", reset multi-line comment state and continue on this line
			else
			{
				(*it).erase(0, pos + 1);
				multiLineCommentState = false;
			}
		}
	}
}

/// <summary>
/// Following method processes all ifdef/else/elif/endif and define, removes parts of code based on this
/// </summary>
/// <param name="defines">Array of defines passed into the compiler as arguments</param>
void ShaderParser::ProcessIfdefs(const std::vector<std::string>& defines)
{
	std::vector<std::string> defs;

	// Loop through all shaders
	for (auto itShader = mParseData.begin(); itShader != mParseData.end(); itShader++)
	{
		// Loop through all versions
		for (auto itVersion = itShader->second.begin(); itVersion != itShader->second.end(); itVersion++)
		{
			// Clear temporary define buffer and add command-line defined defines in it
			defs.clear();
			for (auto it = defines.begin(); it != defines.end(); it++)
			{
				defs.push_back(*it);
			}

			// Helper iterators to help us remove undefined lines
			auto itBeginUndefined = itVersion->second.end();
			auto itEndUndefined = itVersion->second.end();
			auto itPrev = itVersion->second.begin();

			// Are we now in undefined code
			bool undefinedCode = false;
			// Define level
			int defStack = 0;
			// Level where undefined code started
			int undefinedLvl = -1;
			// Are we in undefined state on current line
			bool undefinedState = false;

			// Loop through all lines
			for (auto it = itVersion->second.begin(); it != itVersion->second.end();)
			{
				// Get line type
				LineType lType = GetPreprocessorLineType(*it);

				switch (lType)
				{
					// Add each define into our define list
				case LINE_MACRO_DEFINE:
					if (undefinedCode == false)
					{
						std::string define = GetDefine(*it);
						defs.push_back(define);
						it = itVersion->second.erase(it);
						continue;
					}
					break;

					// Handle #ifdef
				case LINE_MACRO_IFDEF:
					if (undefinedCode == false)
					{
						std::string define = GetDefine(*it);
						if (std::find(defs.begin(), defs.end(), define) == defs.end())
						{
							undefinedCode = true;
							itBeginUndefined = it;
							undefinedLvl = defStack;
						}
						else
						{
							it = itVersion->second.erase(it);
							continue;
						}
					}
					defStack++;
					break;

					// Handle #ifndef
				case LINE_MACRO_IFNDEF:
					if (undefinedCode == false)
					{
						std::string define = GetDefine(*it);
						if (std::find(defs.begin(), defs.end(), define) != defs.end())
						{
							undefinedCode = true;
							itBeginUndefined = it;
							undefinedLvl = defStack;
						}
						else
						{
							it = itVersion->second.erase(it);
							continue;
						}
					}
					defStack++;
					break;

					// Handle #else
				case LINE_MACRO_ELSE:
					if (undefinedCode == true)
					{
						undefinedCode = false;
						itEndUndefined = it;
						undefinedLvl = -1;
						it = itVersion->second.erase(itBeginUndefined, itEndUndefined + 1);
						continue;
					}
					else
					{
						undefinedCode = true;
						itBeginUndefined = it;
						undefinedLvl = defStack - 1;
					}
					break;

					// Handle #elif
				case LINE_MACRO_ELIF:
					{
						std::string define = GetDefine(*it);
						if (std::find(defs.begin(), defs.end(), define) == defs.end())
						{
							if (undefinedCode == false)
							{
								undefinedCode = true;
								itBeginUndefined = it;
								undefinedLvl = defStack - 1;
							}
						}
						else
						{
							if (undefinedCode == true)
							{
								undefinedCode = false;
								itEndUndefined = it;
								undefinedLvl = -1;
								it = itVersion->second.erase(itBeginUndefined, itEndUndefined + 1);
								continue;
							}
						}
					}
					break;

					// Handle #endif
				case LINE_MACRO_ENDIF:
					defStack--;
					if (undefinedLvl == defStack && undefinedCode == true)
					{
						undefinedCode = false;
						itEndUndefined = it;
						undefinedLvl = -1;
						it = itVersion->second.erase(itBeginUndefined, itEndUndefined + 1);
						continue;
					}
					else
					{
						it = itVersion->second.erase(it);
					}
					break;

				default:
					break;
				}

				itPrev = it;
				it++;
			}
		}
	}
}

/// <summary>
/// Opens the shader file, parses it and returns true on success
/// </summary>
/// <param name="lines">All lines from shader file</param>
bool ShaderParser::ParseFile(const std::vector<std::string>& lines)
{
	ShaderType sType;
	int version;

	for (auto it = lines.begin(); it != lines.end(); it++)
	{
		LineType lType = GetPreprocessorLineType(*it);

		switch (lType)
		{
		case LINE_SHADER_SPECIFICATION:
			sType = GetShaderType(*it);
			break;

		case LINE_SHADER_VERSION:
			version = GetVersion(*it);
			mParseData[sType][version].push_back(*it);
			break;

		default:
			mParseData[sType][version].push_back(*it);
			break;
		}
	}

	return true;
}

/// <summary>
/// Gets include file name from line
/// </summary>
/// <param name="line">The line containing #include FILE</param>
std::string ShaderParser::GetInclude(const std::string& line)
{
	size_t absBegin = line.find('<');
	size_t absEnd = line.find('>');

	if (absBegin != std::string::npos && absEnd != std::string::npos)
	{
		return line.substr(absBegin + 1, absEnd - absBegin - 1);
	}

	return "";
}

/// <summary>
/// Preprocesses include files, by copying there content into the file
/// </summary>
void ShaderParser::PreproessIncludes(const std::vector<std::string>& includeDirs)
{
	// Is file in relative folder
	std::string includeName;
	std::string filename;

	// Loop through all shaders
	for (auto itShader = mParseData.begin(); itShader != mParseData.end(); itShader++)
	{
		// Loop through all versions
		for (auto itVersion = itShader->second.begin(); itVersion != itShader->second.end(); itVersion++)
		{
			// Loop through all lines
			for (auto it = itVersion->second.begin(); it != itVersion->second.end(); it++)
			{
				// Get line type
				LineType lType = GetPreprocessorLineType(*it);

				switch (lType)
				{
					// If line is #include
				case LINE_INCLUDE:
					includeName = GetInclude(*it);

					// If included file name has at least length of 1 character
					if (includeName.length() > 0)
					{
						// Check each include directory whether it contains file
						for (auto itDirs = includeDirs.begin(); itDirs != includeDirs.end(); itDirs++)
						{
							// Create full path+filename
							filename = (*itDirs) + includeName;

							// If file exists
							std::ifstream infile(filename);
							if (infile.good())
							{
								// Read whole included file and insert its contents into this file
								std::vector<std::string> lines;
								bool success = ReadFile(filename, lines);
								if (success)
								{
									RemoveComments(lines);
									for (auto itTmp = lines.begin(); itTmp != lines.end(); itTmp++)
									{
										it = itVersion->second.insert(it, *itTmp);
										it++;
									}
									it = itVersion->second.erase(it);
								}
							}
						}
					}
					break;

				default:
					break;
				}
			}
		}
	}
}

/// <summary>
/// Helper function to save shaders
/// </summary>
void ShaderParser::DebugSave()
{
	// Loop through all shaders
	for (auto itShader = mParseData.begin(); itShader != mParseData.end(); itShader++)
	{
		// Loop through all versions
		for (auto itVersion = itShader->second.begin(); itVersion != itShader->second.end(); itVersion++)
		{
			// Create filename and open file
			std::string filename = std::string(ShaderTypeDebugName[itShader->first]) +
				std::string("_") +
				Math::ToString(itVersion->first) +
				std::string(".txt");

			std::ofstream file(filename, std::fstream::out);

			// Loop through all lines and write them
			for (auto it = itVersion->second.begin(); it != itVersion->second.end(); it++)
			{
				file << *it << std::endl;
			}

			// Close file
			file.close();
		}
	}
}

/// <summary>
/// Preprocesses whole shader file
/// </summary>
void ShaderParser::Parse(const std::string& filename, const std::vector<std::string> defines, const std::vector<std::string> dirs)
{
	// Clear previously held data
	mParseData.clear();

	// Are we having absolute path? Then get path, ortherwise get absolute path to the file
	if (filename.find(":") != std::string::npos || filename[0] == '\\' || filename[0] == '/')
	{
		mDirectory = filename.substr(0, filename.find_last_of("/\\") + 1);
	}
	else
	{
		char tmp[255];
		int result = GetModuleFileName(NULL, tmp, 255);
		if (result != -1)
		{
			mDirectory = tmp;
			mDirectory = mDirectory.substr(0, mDirectory.find_last_of("/\\") + 1);
		}
		else
		{
			mDirectory = "";
		}
		std::string filepath = filename.substr(0, filename.find_last_of("/\\") + 1);
		mDirectory += filepath;
	}

	// Push all directories from arguments into temporary container (including directory of shader source)
	std::vector<std::string> directories;
	directories.push_back(mDirectory);
	for (std::string d : dirs)
	{
		directories.push_back(d);
	}

	// Read the file
	std::vector<std::string> lines;
	bool success = ReadFile(filename, lines);

	// If correct, parse it and preprocess it
	if (success)
	{
		RemoveComments(lines);
		ParseFile(lines);
		PreproessIncludes(directories);
		ProcessIfdefs(defines);
	}
}