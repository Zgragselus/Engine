///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Mesh.h
//
// Following file contains class defining basic mesh object
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MESH_H__
#define __MESH_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Graphics/Resources/Buffer/VertexBuffer.h"
#include "Graphics/Resources/Buffer/VertexArray.h"
#include "Graphics/Resources/Buffer/IndexBuffer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// The following class describes mesh object. A common mesh object from which various mesh 
	/// objects inherit.
	/// </summary>
	class Mesh
	{
	protected:
		/// <summary>Vertex buffer of the mesh (holds per-vertex data)</summary>
		VertexBuffer* mVertexBuffer;

		/// <summary>Index buffer of the mesh (holds indices of data)</summary>
		IndexBuffer* mIndexBuffer;

		/// <summary>Vertex array object for the mesh (holds rendering stage information)</summary>
		VertexArray* mVertexArray;

	public:
		/// <summary>Vertex Array attribute structure</summary>
		struct Attribute
		{
			/// <summary>Attribute type</summary>
			enum Type
			{
				FLOAT = 0,
				INT = 1
			};

			/// <summary>Attribute type size - sizeof of each single attribute</summary>
			static size_t mTypeSize[2];

			/// <summary>Attribute type (GLuint)</summary>
			static GLuint mTypeGL[2];

			/// <summary>Channel ID</summary>
			unsigned int mChannelID;

			/// <summary>Number of dimensions for given channel (X - 1, XY - 2, XYZ - 3, XYZW - 4)</summary>
			unsigned int mDimensions;

			/// <summary>Attribute type</summary>
			Type mType;

			/// <summary>Attribute constructor</summary>
			/// <param name="channel">Attribute channel</param>
			/// <param name="dimensions">Attribute dimensions</param>
			/// <param name="size">Size of attribute</param>
			Attribute(unsigned int channel, unsigned int dimensions, Type type) :
				mChannelID(channel),
				mDimensions(dimensions),
				mType(type)
			{
			}
		};

	protected:
		/// <summary>Vertex Array attribute structure - less comparator</summary>
		struct AttributeLess
		{
			bool operator()(const Attribute& a, const Attribute& b)
			{
				return a.mChannelID < b.mChannelID;
			}
		};

	public:
		/// <summary>Mesh constructor</summary>
		/// <param name="vboData">Mesh per-vertex data</param>
		/// <param name="itemSize">Size of single per-vertex item</param>
		/// <param name="itemCount">Number of vertices</param>
		/// <param name="indices">Indices data</param>
		/// <param name="indexCount">Number of indices</param>
		/// <param name="attributeSet">Attribute description (VBO layout)</param>
		Mesh(void* vboData,
			size_t itemSize,
			size_t itemCount,
			unsigned int* indices,
			size_t indexCount,
			std::vector<Attribute> attributeSet);

		/// <summary>Mesh destructor</summary>
		virtual ~Mesh();

		/// <summary>Vertex Array Object getter</summary>
		inline VertexArray* GetVAO()
		{
			return mVertexArray;
		}

		/// <summary>Vertex Buffer Object getter</summary>
		inline VertexBuffer* GetVBO()
		{
			return mVertexBuffer;
		}

		/// <summary>Index Buffer Object getter</summary>
		inline IndexBuffer* GetIBO()
		{
			return mIndexBuffer;
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::Mesh")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
