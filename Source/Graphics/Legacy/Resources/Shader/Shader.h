///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Shader.h
//
// Following file contains shader class
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SHADER__H__
#define __SHADER__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Parser.h"
#include "Core/Files/Files.h"
#include "Core/Log/Log.h"
#include "Core/Core.h"
#include "Graphics/Renderer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Shader class for holding one full shader program
	/// </summary>
	class Shader
	{
	private:
		unsigned long long int mLastWriteTime;	/// Last written time for the shader file (hotplug)

		GLhandleARB mShaderProgram;				/// Shader program handle

		GLhandleARB mVertexShader;				/// Vertex shader handle
		GLhandleARB mTessEvalShader;			/// Tessellation hull shader handle
		GLhandleARB mTessControlShader;			/// Tessellation control shader handle
		GLhandleARB mGeometryShader;			/// Geometry shader handle
		GLhandleARB mPixelShader;				/// Pixel shader handle
		GLhandleARB mComputeShader;				/// Compute shader handle

		Log* mLog;								/// Log output

		/// <summary>Parser for shader file, contains code data of the shader</summary>
		ShaderParser* mParser;

		/// <summary>Temporary helper variable for defines</summary>
		std::vector<std::string> mDefines;

		/// <summary>Temporary helper variable for directories</summary>
		std::vector<std::string> mDirectories;

		/// <summary>Target shader version passed through args</summary>
		int mVersion;

		/// <summary>Shader file name</summary>
		std::string mFilename;

		/// <summary>
		/// Following method parses args into list of definitions and directories.
		/// Definition is given as '-dDEFINITION' (where DEFINITION is what we want to define)
		/// Directory is given as '-IDIRECTORY' (where DIRECTORY specifies the directory
		/// </summary>
		/// <param name="args">Arguments given in as command line params</param>
		void SetCommandLine(const std::string& args);

		/// <summary>
		/// Load shader
		/// </summary>
		void LoadShader(const std::string& filename);

		/// <summary>
		/// Clean shader (release used resources)
		/// </summary>
		void CleanShader();

		/// <summary>
		/// Link shader program
		/// </summary>
		void LinkShader();

		/// <summary>
		/// Compile shader program for given version
		/// </summary>
		/// <param name="version">Version of the shader program</param>
		void CompileShader(int version);

	public:
		/// <summary>
		/// Creates shaders from input file, command line arguments for parser are taken from constants
		/// </summary>
		/// <param name="filename">Shader file</param>
		Shader(const std::string& filename);

		/// <summary>
		/// Creates shaders from input file, takes input arguments into account
		/// </summary>
		/// <param name="filename">Shader file</param>
		/// <param name="args">Command line arguments for parser</param>
		Shader(const std::string& filename, const std::string& args, Log* log);

		/// <summary>
		/// Destructor, cleans up used resources
		/// </summary>
		~Shader();

		/// <summary>
		/// If file has been changed, re-load it (aka hot-plug it on the run)
		/// </summary>
		void Hotplug();

		///////////////////////////////////////////////////////////////////////////////////////////
		// Get variable ID for given parameter name

		/// <summary>Get variable ID for given parameter name</summary>
		/// <param name="mVarName">Variable name in shader</param>
		int GetVariable(const std::string& mVarName)
		{
			if (!this->mShaderProgram)
			{
				return -1;
			}

			return glGetUniformLocation(this->mShaderProgram, mVarName.c_str());
		}

		///////////////////////////////////////////////////////////////////////////////////////////
		// Integer values, vectors and arrays

		/// <summary> Set int to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual value</param>
		void SetInt(int mVariable, int mValue)
		{
			glUniform1i(mVariable, mValue);
		}

		/// <summary> Set int 2D vector to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual X value</param>
		/// <param name="mValue2">Actual Y value</param>
		void SetInt2(int mVariable, int mValue, int mValue2)
		{
			glUniform2i(mVariable, mValue, mValue2);
		}

		/// <summary> Set int 3D vector to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual X value</param>
		/// <param name="mValue2">Actual Y value</param>
		/// <param name="mValue3">Actual Z value</param>
		void SetInt3(int mVariable, int mValue, int mValue2, int mValue3)
		{
			glUniform3i(mVariable, mValue, mValue2, mValue3);
		}

		/// <summary> Set int 4D vector to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual X value</param>
		/// <param name="mValue2">Actual Y value</param>
		/// <param name="mValue3">Actual Z value</param>
		/// <param name="mValue4">Actual W value</param>
		void SetInt4(int mVariable, int mValue, int mValue2, int mValue3, int mValue4)
		{
			glUniform4i(mVariable, mValue, mValue2, mValue3, mValue4);
		}

		/// <summary> Set int array to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of ints in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetIntArray(int mVariable, size_t mCount, int *mArray)
		{
			glUniform1iv(mVariable, (GLsizei)mCount, mArray);
		}

		/// <summary> Set array of 2D int vectors to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of ints in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetInt2Array(int mVariable, size_t mCount, int *mArray)
		{
			glUniform2iv(mVariable, (GLsizei)mCount, mArray);
		}

		/// <summary> Set array of 3D int vectors to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of ints in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetInt3Array(int mVariable, size_t mCount, int *mArray)
		{
			glUniform3iv(mVariable, (GLsizei)mCount, mArray);
		}

		/// <summary> Set array of 4D int vectors to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of ints in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetInt4Array(int mVariable, size_t mCount, int *mArray)
		{
			glUniform4iv(mVariable, (GLsizei)mCount, mArray);
		}

		///////////////////////////////////////////////////////////////////////////////////////////
		// Unsigned int values, vectors and arrays

		/// <summary> Set unsigned int to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual value</param>
		void SetUInt(int mVariable, unsigned int mValue)
		{
			glUniform1ui(mVariable, mValue);
		}

		/// <summary> Set unsigned int 2D vector to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual X value</param>
		/// <param name="mValue2">Actual Y value</param>
		void SetUInt2(int mVariable, unsigned int mValue, unsigned int mValue2)
		{
			glUniform2ui(mVariable, mValue, mValue2);
		}

		/// <summary> Set unsigned int 3D vector to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual X value</param>
		/// <param name="mValue2">Actual Y value</param>
		/// <param name="mValue3">Actual Z value</param>
		void SetUInt3(int mVariable, unsigned int mValue, unsigned int mValue2, unsigned int mValue3)
		{
			glUniform3ui(mVariable, mValue, mValue2, mValue3);
		}

		/// <summary> Set unsigned int 4D vector to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual X value</param>
		/// <param name="mValue2">Actual Y value</param>
		/// <param name="mValue3">Actual Z value</param>
		/// <param name="mValue4">Actual W value</param>
		void SetUInt4(int mVariable, unsigned int mValue, unsigned int mValue2, unsigned int mValue3, unsigned int mValue4)
		{
			glUniform4ui(mVariable, mValue, mValue2, mValue3, mValue4);
		}

		/// <summary> Set unsigned int array to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of unsigned ints in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetUIntArray(int mVariable, size_t mCount, unsigned int *mArray)
		{
			glUniform1uiv(mVariable, (GLsizei)mCount, mArray);
		}

		/// <summary> Set array of 2D unsigned int vectors to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of unsigned ints in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetUInt2Array(int mVariable, size_t mCount, unsigned int *mArray)
		{
			glUniform2uiv(mVariable, (GLsizei)mCount, mArray);
		}

		/// <summary> Set array of 3D unsigned int vectors to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of unsigned ints in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetUInt3Array(int mVariable, size_t mCount, unsigned int *mArray)
		{
			glUniform3uiv(mVariable, (GLsizei)mCount, mArray);
		}

		/// <summary> Set array of 4D unsigned int vectors to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of unsigned ints in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetUInt4Array(int mVariable, size_t mCount, unsigned int *mArray)
		{
			glUniform4uiv(mVariable, (GLsizei)mCount, mArray);
		}

		///////////////////////////////////////////////////////////////////////////////////////////
		// Float precision values, vectors and arrays

		/// <summary> Set float to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual value</param>
		void SetFloat(int mVariable, float mValue)
		{
			glUniform1f(mVariable, mValue);
		}

		/// <summary> Set float 2D vector to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual X value</param>
		/// <param name="mValue2">Actual Y value</param>
		void SetFloat2(int mVariable, float mValue, float mValue2)
		{
			glUniform2f(mVariable, mValue, mValue2);
		}

		/// <summary> Set float 3D vector to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual X value</param>
		/// <param name="mValue2">Actual Y value</param>
		/// <param name="mValue3">Actual Z value</param>
		void SetFloat3(int mVariable, float mValue, float mValue2, float mValue3)
		{
			glUniform3f(mVariable, mValue, mValue2, mValue3);
		}

		/// <summary> Set float 4D vector to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual X value</param>
		/// <param name="mValue2">Actual Y value</param>
		/// <param name="mValue3">Actual Z value</param>
		/// <param name="mValue4">Actual W value</param>
		void SetFloat4(int mVariable, float mValue, float mValue2, float mValue3, float mValue4)
		{
			glUniform4f(mVariable, mValue, mValue2, mValue3, mValue4);
		}

		/// <summary> Set float array to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of floats in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetFloatArray(int mVariable, size_t mCount, float* mArray)
		{
			glUniform1fv(mVariable, (GLsizei)mCount, mArray);
		}

		/// <summary> Set array of 2D float vectors to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of floats in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetFloat2Array(int mVariable, size_t mCount, float* mArray)
		{
			glUniform2fv(mVariable, (GLsizei)mCount, mArray);
		}

		/// <summary> Set array of 3D float vectors to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of floats in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetFloat3Array(int mVariable, size_t mCount, float* mArray)
		{
			glUniform3fv(mVariable, (GLsizei)mCount, mArray);
		}

		/// <summary> Set array of 4D float vectors to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of floats in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetFloat4Array(int mVariable, size_t mCount, float* mArray)
		{
			glUniform4fv(mVariable, (GLsizei)mCount, mArray);
		}

		///////////////////////////////////////////////////////////////////////////////////////////
		// Double precision values, vectors and arrays

		/// <summary> Set double to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual value</param>
		void SetDouble(int mVariable, double mValue)
		{
			glUniform1d(mVariable, mValue);
		}

		/// <summary> Set double 2D vector to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual X value</param>
		/// <param name="mValue2">Actual Y value</param>
		void SetDouble2(int mVariable, double mValue, double mValue2)
		{
			glUniform2d(mVariable, mValue, mValue2);
		}

		/// <summary> Set double 3D vector to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual X value</param>
		/// <param name="mValue2">Actual Y value</param>
		/// <param name="mValue3">Actual Z value</param>
		void SetDouble3(int mVariable, double mValue, double mValue2, double mValue3)
		{
			glUniform3d(mVariable, mValue, mValue2, mValue3);
		}

		/// <summary> Set double 4D vector to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mValue">Actual X value</param>
		/// <param name="mValue2">Actual Y value</param>
		/// <param name="mValue3">Actual Z value</param>
		/// <param name="mValue4">Actual W value</param>
		void SetDouble4(int mVariable, double mValue, double mValue2, double mValue3, double mValue4)
		{
			glUniform4d(mVariable, mValue, mValue2, mValue3, mValue4);
		}

		/// <summary> Set double array to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of doubles in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetDoubleArray(int mVariable, size_t mCount, double* mArray)
		{
			glUniform1dv(mVariable, (GLsizei)mCount, mArray);
		}

		/// <summary> Set array of 2D double vectors to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of doubles in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetDouble2Array(int mVariable, size_t mCount, double* mArray)
		{
			glUniform2dv(mVariable, (GLsizei)mCount, mArray);
		}

		/// <summary> Set array of 3D double vectors to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of doubles in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetDouble3Array(int mVariable, size_t mCount, double* mArray)
		{
			glUniform3dv(mVariable, (GLsizei)mCount, mArray);
		}

		/// <summary> Set array of 4D double vectors to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mCount">Number of doubles in array</param>
		/// <param name="mArray">Pointer to data</param>
		void SetDouble4Array(int mVariable, size_t mCount, double* mArray)
		{
			glUniform4dv(mVariable, (GLsizei)mCount, mArray);
		}

		///////////////////////////////////////////////////////////////////////////////////////////
		// Single-Precision Matrices

		/// <summary> Set 2x2 matrix to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mNumMatrices">Number of matrices to pass in</param>
		/// <param name="mTranspose">Do we want to transpose matrix</param>
		/// <param name="mMatrix">Pointer to matrix data</param>
		void SetMat2(int mVariable, int mNumMatrices, bool mTranspose, float* mMatrix)
		{
			glUniformMatrix2fv(mVariable, mNumMatrices, mTranspose, mMatrix);
		}

		/// <summary> Set 3x3 matrix to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mNumMatrices">Number of matrices to pass in</param>
		/// <param name="mTranspose">Do we want to transpose matrix</param>
		/// <param name="mMatrix">Pointer to matrix data</param>
		void SetMat3(int mVariable, int mNumMatrices, bool mTranspose, float* mMatrix)
		{
			glUniformMatrix3fv(mVariable, mNumMatrices, mTranspose, mMatrix);
		}

		/// <summary> Set 4x4 matrix to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mNumMatrices">Number of matrices to pass in</param>
		/// <param name="mTranspose">Do we want to transpose matrix</param>
		/// <param name="mMatrix">Pointer to matrix data</param>
		void SetMat4(int mVariable, int mNumMatrices, bool mTranspose, float* mMatrix)
		{
			glUniformMatrix4fv(mVariable, mNumMatrices, mTranspose, mMatrix);
		}

		///////////////////////////////////////////////////////////////////////////////////////////
		// Double-Precision Matrices

		/// <summary> Set 2x2 double precision matrix to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mNumMatrices">Number of matrices to pass in</param>
		/// <param name="mTranspose">Do we want to transpose matrix</param>
		/// <param name="mMatrix">Pointer to matrix data</param>
		void SetMat2(int mVariable, int mNumMatrices, bool mTranspose, double* mMatrix)
		{
			glUniformMatrix2dv(mVariable, mNumMatrices, mTranspose, mMatrix);
		}

		/// <summary> Set 3x3 double precision matrix to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mNumMatrices">Number of matrices to pass in</param>
		/// <param name="mTranspose">Do we want to transpose matrix</param>
		/// <param name="mMatrix">Pointer to matrix data</param>
		void SetMat3(int mVariable, int mNumMatrices, bool mTranspose, double* mMatrix)
		{
			glUniformMatrix3dv(mVariable, mNumMatrices, mTranspose, mMatrix);
		}

		/// <summary> Set 4x4 double precision matrix to variable </summary>
		/// <param name="mVariable">Variable ID in shader</param>
		/// <param name="mNumMatrices">Number of matrices to pass in</param>
		/// <param name="mTranspose">Do we want to transpose matrix</param>
		/// <param name="mMatrix">Pointer to matrix data</param>
		void SetMat4(int mVariable, int mNumMatrices, bool mTranspose, double* mMatrix)
		{
			glUniformMatrix4dv(mVariable, mNumMatrices, mTranspose, mMatrix);
		}

		///////////////////////////////////////////////////////////////////////////////////////////
		// On/Off functions

		/// <summary> Turn the shader on </summary>
		void TurnOn()
		{
			glUseProgram(this->mShaderProgram);
		}

		/// <summary> Turn the shader off </summary>
		void TurnOff()
		{
			glUseProgram(0);
		}

		///////////////////////////////////////////////////////////////////////////////////////////
		// Uniform buffer functions

		/// <summary>Get uniform buffer block index</summary>
		/// <param name="blockName">Name of UBO block</param>
		unsigned int GetUniformBlockIndex(const std::string& blockName)
		{
			return glGetUniformBlockIndex(this->mShaderProgram, blockName.c_str());
		}

		/// <summary>Set binding ID of UBO</summary>
		/// <param name="blockIndex">Index of block</param>
		/// <param name="bindingPoint">Binding point</param>
		void SetUniformBlockBinding(unsigned int blockIndex, unsigned int bindingPoint)
		{
			glUniformBlockBinding(this->mShaderProgram, blockIndex, bindingPoint);
		}
	};
}

#endif