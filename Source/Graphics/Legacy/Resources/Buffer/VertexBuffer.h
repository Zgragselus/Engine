///////////////////////////////////////////////////////////////////////////////////////////////////
//
// VertexBuffer.h
//
// Following file defines OpenGL VertexBuffer class (Vertex Buffer Objects)
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __VERTEX_BUFFER__H__
#define __VERTEX_BUFFER__H__

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
	class VertexBuffer
	{
	protected:
		/// <summary>Pointer to actual data</summary>
		void* data;
		/// <summary>Sizeof single item inside array</summary>
		size_t itemSize;
		/// <summary>Total item count</summary>
		size_t items;
		/// <summary>Sizeof whole buffer</summary>
		size_t size;

		/// <summary>OpenGL Resource ID for this buffer</summary>
		GLuint glData;
		/// <summary>Update the buffer?</summary>
		bool update;

	public:
		/// <summary>
		/// Constructor for VertexBuffer
		/// </summary>
		/// <param name="ptr">Buffer data</param>
		/// <param name="sizeOfItem">Sizeof single element</param>
		/// <param name="noOfElements">Number of elements in buffer</param>
		/// <param name="usage">How will we use buffer (OpenGL VBO usage)</param>
		VertexBuffer(void* ptr, size_t sizeOfItem, size_t noOfElements, GLenum usage)
		{
			itemSize = sizeOfItem;
			items = noOfElements;
			size = itemSize * items;
			data = SharedMalloc(size, 16, "Engine::Buffer::data");
			if (ptr != NULL)
			{
				memcpy(data, ptr, size);
			}

			glGenBuffers(1, &glData);
			glBindBuffer(GL_ARRAY_BUFFER, glData);
			glBufferData(GL_ARRAY_BUFFER, size, data, usage);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		/// <summary>
		/// Destructor for VertexBuffer
		/// </summary>
		virtual ~VertexBuffer()
		{
			SharedFree(data, "Engine::Buffer::data");

			glDeleteBuffers(1, &glData);
		}

		/// <summary>
		/// Templated element access operator
		/// </summary>
		/// <param name="i">Index</param>
		template <typename T>
		T& operator[](int i)
		{
			return ((T*)data)[i];
		}
	
		/// <summary>
		/// Templated element access operator (const version)
		/// </summary>
		/// <param name="i">Index</param>
		template <typename T>
		const T& operator[](int i)
		{
			return ((const T*)data)[i];
		}

		/// <summary>
		/// Synchronize between device and host
		/// </summary>
		/// <param name="deviceToHost">Prefer GPU memory (GPU->CPU) or CPU memory (CPU->GPU)</param>
		void Synchronize(bool deviceToHost)
		{
			if (deviceToHost)
			{
				glBindBuffer(GL_ARRAY_BUFFER, glData);
				void* gpuPtr = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
				memcpy(data, gpuPtr, size);
				glUnmapBuffer(GL_ARRAY_BUFFER);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			else
			{
				glBindBuffer(GL_ARRAY_BUFFER, glData);
				void* gpuPtr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
				memcpy(gpuPtr, data, size);
				glUnmapBuffer(GL_ARRAY_BUFFER);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
		}

		/// <summary>
		/// Get pointer to data
		/// </summary>
		template <typename T>
		T* GetPtr()
		{
			return (T*)data;
		}

		/// <summary>
		/// Get count of items
		/// </summary>
		size_t Count()
		{
			return items;
		}

		/// <summary>
		/// Enable the vertex buffer object
		/// </summary>
		void Enable()
		{
			glBindBuffer(GL_ARRAY_BUFFER, glData);
		}

		/// <summary>
		/// Disable the vertex buffer object
		/// </summary>
		void Disable()
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		/// <summary>
		/// Draw vertex buffer object
		/// </summary>
		/// <param name="mode">OpenGL primitive rendering mode</param>
		void Draw(GLenum mode)
		{
			glDrawArrays(mode, 0, (GLsizei)items);
		}

		/// <summary>
		/// Indexed draw
		/// </summary>
		/// <param name="mode">OpenGL primitive rendering mode</param>
		/// <param name="indicesCount">Number of indices</param>
		void DrawElements(GLenum mode, size_t indicesCount)
		{
			glDrawElements(mode, (GLsizei)indicesCount, GL_UNSIGNED_INT, 0);
		}

		/// <summary>
		/// Draw vertex buffer object begin on specified primitive and render exact count of them
		/// </summary>
		/// <param name="mode">OpenGL primitive rendering mode</param>
		/// <param name="offset">Starting index of array</param>
		/// <param name="count">How many indices we render</param>
		void DrawCount(GLenum mode, GLint offset, GLsizei count)
		{
			glDrawArrays(mode, offset, count);
		}

		/// <summary>
		/// Draw vertex buffer object using instancing (e.g. N-times)
		/// </summary>
		/// <param name="mode">OpenGL primitive rendering mode</param>
		/// <param name="primcount">Number of times, the array is instanced</param>
		void DrawInstanced(GLenum mode, GLint primcount)
		{
			glDrawArraysInstanced(mode, 0, (GLsizei)items, primcount);
		}

		/// <summary>
		/// Indexed draw instanced
		/// </summary>
		/// <param name="mode">OpenGL primitive rendering mode</param>
		/// <param name="indicesCount">Number of indices</param>
		/// <param name="primcount">Number of times, the array is instanced</param>
		void DrawElements(GLenum mode, size_t indicesCount, GLint primcount)
		{
			glDrawElementsInstanced(mode, (GLsizei)indicesCount, GL_UNSIGNED_INT, 0, primcount);
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::VertexBuffer")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif