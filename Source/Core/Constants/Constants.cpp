///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constants.cpp
//
// Following file implements methods defined in Constants.h.
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

#include "Constants.h"
#include "Core/Log/Log.h"
#include <fstream>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Declaration section

using namespace Engine;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

/// <summary>Creates Constants object, loads constants from file</summary>
/// <param name="l">Pointer to log class</param>
/// <param name="filename">File containing constants</param>
Constants::Constants(Log* l, const std::string& filename)
{
	mLog = l;

	std::ifstream f;
	f.open(filename);

	if (f.is_open())
	{
		mCurrentLine = 0;
		while (f.good() && !f.eof())
		{
			mCurrentLine++;
			std::string line;
			std::getline(f, line);
			ParseLine(line);
		}

		f.close();

		mLog->Print("Constants", std::string("File ") + filename + std::string(" successfully loaded!"));
	}
	else
	{
		mLog->Print("Constants", std::string("File ") + filename + std::string(" does NOT exist!"));
	}
}

/// <summary>Destructor, removes all used data</summary>
Constants::~Constants()
{
	for (std::map<std::string, sConstant*>::iterator it = mData.begin(); it != mData.end(); it++)
	{
		delete it->second;
	}

	mData.clear();
}

/// <summary>
/// Parses the line in following file, determines between blank lines, block lines, 
/// comments and constant lines.
/// </summary>
/// <param name="line">Line string</param>
void Constants::ParseLine(const std::string& line)
{
	// Trim line
	std::string lineTrimmed = line;
	String::Trim(lineTrimmed);

	// If line contains nothing, return
	if (lineTrimmed.length() < 1)
	{
		return;
	}

	// If line is comment, return
	if (lineTrimmed[0] == '/' && lineTrimmed[1] == '/')
	{
		return;
	}

	// If line contains separator, it is constant line
	if (lineTrimmed.find('=') != std::string::npos)
	{
		ParseConstantLine(lineTrimmed);
		return;
	}

	// If we're beginning new block, continue
	if (lineTrimmed.find('{') != std::string::npos)
	{
		return;
	}

	// If we're ending previous block, jump 1 level up
	if (lineTrimmed.find('}') != std::string::npos)
	{
		if (mLevel.size() == 0)
		{
			mLog->Print("Constants", std::string("Parser error at line: ") + Math::ToString(mCurrentLine) + std::string(" - Error: Unexpected scope end"));
		}
		mLevel.pop_back();
		return;
	}

	// Otherwise we have a keyword describing next block
	mLevel.push_back(lineTrimmed);
}

/// <summary>Parses following line holding constant, stores into data</summary>
/// <param name="line">String holding line to parse, trimmed</param>
void Constants::ParseConstantLine(const std::string& line)
{
	// Separate line by equal sign
	size_t separator = line.find('=');
	std::string left = line.substr(0, separator);
	std::string right = line.substr(separator + 1);

	// Trim both sides
	String::Trim(left);
	String::Trim(right);

	// Create name
	std::string name;
	for (auto& level : mLevel)
	{
		name += level;
		name += ".";
	}
	name += left;

	// Detect double-definition of name
	std::map<std::string, sConstant*>::iterator it = mData.find(name);
	if (it != mData.end())
	{
		mLog->Print("Constants", std::string("Parser error at line: ") + Math::ToString(mCurrentLine) + std::string(" - Error: Value already defined"));
	}

	// Create value type
	tConstant type = ParseType(right);

	// Set the values
	switch (type)
	{
	case CONSTANT_FLOAT:
		Set<float>(name, Math::ToFloat(right));
		break;

	case CONSTANT_INT:
		Set<int>(name, Math::ToInteger(right));
		break;

	case CONSTANT_STRING:
		Set<std::string>(name, right.substr(1, right.length() - 2));
		break;

	case CONSTANT_FLOAT4:
		std::string rightsub = right.substr(right.find_first_of('(') + 1);
		rightsub = rightsub.substr(0, rightsub.find_last_of(')') - 1);
		std::vector<std::string> tokens;
		String::Split(rightsub, ',', tokens);
		for (auto it = tokens.begin(); it != tokens.end(); it++)
		{
			String::Trim(*it);
		}
		Set<float4>(name, float4(	Math::ToFloat(tokens[0]),
									Math::ToFloat(tokens[1]),
									Math::ToFloat(tokens[2]),
									Math::ToFloat(tokens[3])));
		break;
	}
}

/// <summary>
/// Parses line containing either value or string, determining whether we're using float, 
/// integer or string.
/// </summary>
/// <param name="line">String containing value</param>
Constants::tConstant Constants::ParseType(const std::string& line)
{
	if (Math::IsInteger(line))
	{
		return CONSTANT_INT;
	}
	else if (Math::IsFloat(line))
	{
		return CONSTANT_FLOAT;
	}
	else if (IsString(line))
	{
		return CONSTANT_STRING;
	}
	else if (IsFloat4(line))
	{
		return CONSTANT_FLOAT4;
	}
	else
	{
		mLog->Print("Constants", std::string("Parser error at line: ") + Math::ToString(mCurrentLine) + std::string(" - Error: Unknown value type"));
		return CONSTANT_STRING;
	}
} 