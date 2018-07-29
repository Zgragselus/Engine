///////////////////////////////////////////////////////////////////////////////////////////////////
//
// VertexArray.h
//
// Following file defines OpenGL VertexArray class (Vertex Array Objects), aka control object
// for VertexBuffer binding.
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __VERTEX_ARRAY__H__
#define __VERTEX_ARRAY__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Memory/Memory.h"
#include "Graphics/Renderer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Small VertexBuffer class that holds data buffer and allows to synchronize data between CPU
	/// memory and GPU (OpenGL) memory
	/// </summary>
	class VertexArray
	{
	private:
		/// <summary>OpenGL Resource ID for this buffer</summary>
		GLuint glData;

	public:
		/// <summary>
		/// Constructor for VertexArray
		/// </summary>
		VertexArray()
		{
			glGenVertexArrays(1, &glData);
		}

		/// <summary>
		/// Destructor for VertexArray
		/// </summary>
		virtual ~VertexArray()
		{
			glDeleteVertexArrays(1, &glData);
		}

		/// <summary>
		/// Bind VAO
		/// </summary>
		void Enable()
		{
			glBindVertexArray(glData);
		}

		/// <summary>
		/// Unbind VAO
		/// </summary>
		void Disable()
		{
			glBindVertexArray(0);
		}

		/// <summary>
		/// Enable VAO at index
		/// </summary>
		/// <param name="index">Index where we are enabling VAO</param>
		void EnableAttribute(unsigned int index)
		{
			glEnableVertexAttribArray(index);
		}

		/// <summary>
		/// Disable VAO at index
		/// </summary>
		/// <param name="index">Index where we are disabling VAO</param>
		void DisableAttribute(unsigned int index)
		{
			glDisableVertexAttribArray(index);
		}

		/// <summary>
		/// Set VAO attribute
		/// </summary>
		/// <param name="index">Index location</param>
		/// <param name="size">Size of attribute</param>
		/// <param name="type">Value type of attribute</param>
		/// <param name="normalized">Is attribute normalized at [-1;1]</param>
		/// <param name="stride">Stride for the attribute</param>
		/// <param name="pointer">Byte offset into VertexBuffer</param>
		void SetAttribute(unsigned int index, int size, int type, bool normalized, size_t stride, void* pointer)
		{
			glVertexAttribPointer(index, size, type, normalized, (GLsizei)stride, pointer);
		}

		/// <summary>
		/// Set VAO integer attribute attribute
		/// </summary>
		/// <param name="index">Index location</param>
		/// <param name="size">Size of attribute</param>
		/// <param name="type">Value type of attribute</param>
		/// <param name="stride">Stride for the attribute</param>
		/// <param name="pointer">Byte offset into VertexBuffer</param>
		void SetAttributeInteger(unsigned int index, int size, int type, size_t stride, void* pointer)
		{
			glVertexAttribIPointer(index, size, type, (GLsizei)stride, pointer);
		}

		/// <summary>
		/// Set VAO attribute divisor, if divisor is non-zero attribute advances for each instance, otherwise for each vertex
		/// </summary>
		/// <param name="index">Index location</param>
		/// <param name="divisor">Divisor size - 0=vertices, larger than 0 defines number of objects sharing single value</param>
		void SetAttributeDivisor(unsigned int index, unsigned int divisor)
		{
			glVertexAttribDivisor(index, divisor);
		}

		/// <summary>
		/// Get max available vertex attributes supported
		/// </summary>
		static int GetMaxVertexAttributes()
		{
			int result;
			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &result);
			return result;
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::VertexArray")
	};
}

#endif