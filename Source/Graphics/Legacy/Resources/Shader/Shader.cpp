///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Shader.cpp
//
// Following file contains shader class definition
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

#include "Shader.h"
#include "Core/Util/String.h"
#include "Core/Math/Numeric/Common.h"
#include <fstream>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

using namespace Engine;

/// <summary>
/// Following method parses args into list of definitions and directories.
/// Definition is given as '-dDEFINITION' (where DEFINITION is what we want to define)
/// Directory is given as '-IDIRECTORY' (where DIRECTORY specifies the directory
/// </summary>
/// <param name="args">Arguments given in as command line params</param>
/// <param name="defs">Out definitions</param>
/// <param name="dirs">Out directories</param>
void Shader::SetCommandLine(const std::string& args)
{
	std::vector<std::string> tokens = String::Split(args, '-');

	for (std::string& s : tokens)
	{
		if (s[0] == 'd')
		{
			std::string define = s.substr(1);
			String::Trim(define);
			mDefines.push_back(define);
		}
		else if (s[0] == 'I')
		{
			std::string directory = s.substr(1);
			String::Trim(directory);
			mDirectories.push_back(directory);
		}
		else if (s[0] == 'v')
		{
			std::string version = s.substr(1);
			String::Trim(version);
			mVersion = Math::ToInteger(version);
		}
	}
}

/// <summary>
/// Load shader
/// </summary>
void Shader::LoadShader(const std::string& filename)
{
	mParser->Parse(filename, mDefines, mDirectories);
}

/// <summary>
/// Clean shader (release used resources)
/// </summary>
void Shader::CleanShader()
{
	for (auto itShader = mParser->mParseData.begin(); itShader != mParser->mParseData.end(); itShader++)
	{
		switch ((*itShader).first)
		{
		case ShaderParser::SHADER_VERTEX:
			glDetachShader(mShaderProgram, mVertexShader);
			glDeleteShader(mVertexShader);
			break;

		case ShaderParser::SHADER_TESS_CONTROL:
			glDetachShader(mShaderProgram, mTessControlShader);
			glDeleteShader(mTessControlShader);
			break;

		case ShaderParser::SHADER_TESS_EVAL:
			glDetachShader(mShaderProgram, mTessEvalShader);
			glDeleteShader(mTessEvalShader);
			break;

		case ShaderParser::SHADER_GEOMETRY:
			glDetachShader(mShaderProgram, mGeometryShader);
			glDeleteShader(mGeometryShader);
			break;

		case ShaderParser::SHADER_FRAGMENT:
			glDetachShader(mShaderProgram, mPixelShader);
			glDeleteShader(mPixelShader);
			break;

		case ShaderParser::SHADER_COMPUTE:
			glDetachShader(mShaderProgram, mComputeShader);
			glDeleteShader(mComputeShader);
			break;
		}
	}

	glDeleteProgram(mShaderProgram);
}

/// <summary>
/// Link shader program
/// </summary>
void Shader::LinkShader()
{
	glLinkProgram(mShaderProgram);
	int value;
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &value);
	if (!value)
	{
		char log[10240];
		int length;
		glGetProgramInfoLog(mShaderProgram, 10239, &length, log);
		mLog->Print("Shader", std::string("Error linking shader:") + log);
	}
}

/// <summary>
/// Compile shader program for given version
/// </summary>
/// <param name="version">Version of the shader program</param>
void Shader::CompileShader(int version)
{
	mShaderProgram = glCreateProgram();

	glBindAttribLocation(mShaderProgram, 0, "Sc_Vertex");
	glBindAttribLocation(mShaderProgram, 1, "Sc_Normal");
	glBindAttribLocation(mShaderProgram, 2, "Sc_Tangent");
	glBindAttribLocation(mShaderProgram, 3, "Sc_Bitangent");
	glBindAttribLocation(mShaderProgram, 4, "Sc_TexCoord0");
	glBindAttribLocation(mShaderProgram, 5, "Sc_TexCoord1");
	glBindAttribLocation(mShaderProgram, 6, "Sc_Color");

	for (auto itShader = mParser->mParseData.begin(); itShader != mParser->mParseData.end(); itShader++)
	{
		std::string joined = String::Join((*itShader).second[version], '\n');
		const char *source = joined.c_str();
		size_t length = joined.length();
		int value;
		switch ((*itShader).first)
		{
		case ShaderParser::SHADER_VERTEX:
			mVertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(mVertexShader, 1, &source, NULL);
			glCompileShader(mVertexShader);
			glGetShaderiv(mVertexShader, GL_COMPILE_STATUS, &value);
			if (!value)
			{
				char log[10240];
				int length;
				glGetShaderInfoLog(mVertexShader, 10239, &length, log);
				mLog->Print("Shader", std::string("Error compiling vertex shader:") + log);
			}
			glAttachShader(mShaderProgram, mVertexShader);
			break;

		case ShaderParser::SHADER_TESS_CONTROL:
			mTessControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
			glShaderSource(mTessControlShader, 1, &source, NULL);
			glCompileShader(mTessControlShader);
			glGetShaderiv(mTessControlShader, GL_COMPILE_STATUS, &value);
			if (!value)
			{
				char log[10240];
				int length;
				glGetShaderInfoLog(mTessControlShader, 10239, &length, log);
				mLog->Print("Shader", std::string("Error compiling tessellation control shader:") + log);
			}
			glAttachShader(mShaderProgram, mTessControlShader);
			break;

		case ShaderParser::SHADER_TESS_EVAL:
			mTessEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
			glShaderSource(mTessEvalShader, 1, &source, NULL);
			glCompileShader(mTessEvalShader);
			glGetShaderiv(mTessEvalShader, GL_COMPILE_STATUS, &value);
			if (!value)
			{
				char log[10240];
				int length;
				glGetShaderInfoLog(mTessEvalShader, 10239, &length, log);
				mLog->Print("Shader", std::string("Error compiling tessellation evaluation shader:") + log);
			}
			glAttachShader(mShaderProgram, mTessEvalShader);
			break;

		case ShaderParser::SHADER_GEOMETRY:
			mGeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(mGeometryShader, 1, &source, NULL);
			glCompileShader(mGeometryShader);
			glGetShaderiv(mGeometryShader, GL_COMPILE_STATUS, &value);
			if (!value)
			{
				char log[10240];
				int length;
				glGetShaderInfoLog(mGeometryShader, 10239, &length, log);
				mLog->Print("Shader", std::string("Error compiling geometry shader:") + log);
			}
			glAttachShader(mShaderProgram, mGeometryShader);
			break;

		case ShaderParser::SHADER_FRAGMENT:
			mPixelShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(mPixelShader, 1, &source, NULL);
			glCompileShader(mPixelShader);
			glGetShaderiv(mPixelShader, GL_COMPILE_STATUS, &value);
			if (!value)
			{
				char log[10240];
				int length;
				glGetShaderInfoLog(mPixelShader, 10239, &length, log);
				mLog->Print("Shader", std::string("Error compiling fragment shader:") + log);
			}
			glAttachShader(mShaderProgram, mPixelShader);
			break;

		case ShaderParser::SHADER_COMPUTE:
			mComputeShader = glCreateShader(GL_COMPUTE_SHADER);
			glShaderSource(mComputeShader, 1, &source, NULL);
			glCompileShader(mComputeShader);
			glGetShaderiv(mComputeShader, GL_COMPILE_STATUS, &value);
			if (!value)
			{
				char log[10240];
				int length;
				glGetShaderInfoLog(mComputeShader, 10239, &length, log);
				mLog->Print("Shader", std::string("Error compiling compute shader:") + log);
			}
			glAttachShader(mShaderProgram, mComputeShader);
			break;
		}
	}
}

/// <summary>
/// Creates shaders from input file, command line arguments for parser are taken from constants
/// </summary>
/// <param name="filename">Shader file</param>
Shader::Shader(const std::string& filename)
{
	mLog = Core::Instance()->GetLog();
	mFilename = filename;
	mLastWriteTime = Files::GetWriteTime(filename);
	mParser = new ShaderParser(mLog);
	SetCommandLine(Core::Instance()->GetOptions()->Get<std::string>("Graphics.ShaderParser.CommandLineArgs"));
	mParser->Parse(filename, mDefines, mDirectories);
	LoadShader(filename);
	CompileShader(mVersion);
	LinkShader();
}

/// <summary>
/// Creates shaders from input file, takes input arguments into account
/// </summary>
/// <param name="filename">Shader file</param>
Shader::Shader(const std::string& filename, const std::string& args, Log* log)
{
	mLog = log;
	mFilename = filename;
	mLastWriteTime = Files::GetWriteTime(filename);
	mParser = new ShaderParser(mLog);
	SetCommandLine(args);
	mParser->Parse(filename, mDefines, mDirectories);
	LoadShader(filename);
	CompileShader(mVersion);
	LinkShader();
}

/// <summary>
/// Destructor, cleans up used resources
/// </summary>
Shader::~Shader()
{
	CleanShader();
	delete mParser;
}

/// <summary>
/// If file has been changed, re-load it (aka hot-plug it on the run)
/// </summary>
void Shader::Hotplug()
{
	unsigned long long int lastWriteTime = Files::GetWriteTime(mFilename);
	if (lastWriteTime != mLastWriteTime)
	{
		mLastWriteTime = lastWriteTime;
		CleanShader();
		mParser->Parse(mFilename, mDefines, mDirectories);
		LoadShader(mFilename);
		CompileShader(mVersion);
		LinkShader();
	}
}