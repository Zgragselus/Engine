///////////////////////////////////////////////////////////////////////////////////////////////////
//
// UniformBuffer.h
//
// Following file defines OpenGL UniformBuffer class (Uniform Buffer Objects)
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __UNIFORM_BUFFER__H__
#define __UNIFORM_BUFFER__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Memory/Memory.h"
#include "Graphics/Renderer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Small UniformBuffer class that holds data later passed as UBO into shader
	/// </summary>
	class UniformBuffer
	{
	private:
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
		/// Constructor for UniformBuffer
		/// </summary>
		/// <param name="ptr">Buffer data</param>
		/// <param name="sizeOfItem">Sizeof single element</param>
		/// <param name="noOfElements">Number of elements in buffer</param>
		/// <param name="usage">How will we use buffer (OpenGL VBO usage)</param>
		UniformBuffer(void* ptr, size_t sizeOfItem, size_t noOfElements, GLenum usage)
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
			glBindBuffer(GL_UNIFORM_BUFFER, glData);
			glBufferData(GL_UNIFORM_BUFFER, size, NULL, usage);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		/// <summary>
		/// Destructor for UniformBuffer
		/// </summary>
		virtual ~UniformBuffer()
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
		/// Enable the uniform buffer object
		/// </summary>
		void Enable(unsigned int blockIndex)
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, glData);
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::UniformBuffer")
	};
}

#endif